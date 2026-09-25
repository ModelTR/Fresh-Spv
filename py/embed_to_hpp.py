#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
自动将多个 shader 目录（递归）下的 .spv 文件嵌入到 C++ 头文件 (embed.hpp) 中。
生成宏定义、字节数组、span 变量，并汇总为 constexpr SpvsMap (frozen::unordered_map)。
支持多个目录，自动解决文件名冲突（基于相对路径）。
"""

from __future__ import annotations

import sys
import re
import argparse
import logging
from pathlib import Path
from typing import List, Dict, Set, Tuple, Any
from datetime import datetime

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")


# ---------- 命名转换工具 ----------
class NamingConverter:
    """提供命名风格转换功能（可自定义）"""

    @staticmethod
    def camel_to_snake(name: str) -> str:
        """驼峰转蛇形小写，如 'BloomDownsample' -> 'bloom_downsample'"""
        s: str = re.sub(r'(?<!^)(?=[A-Z])', '_', name).lower()
        return s

    @staticmethod
    def sanitize_identifier(name: str) -> str:
        """清理非法字符，确保成为合法的 C++ 标识符"""
        sanitized: str = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        sanitized = re.sub(r'_+', '_', sanitized)
        if sanitized and sanitized[0].isdigit():
            sanitized = '_' + sanitized
        return sanitized


# ---------- 生成器核心类 ----------
class EmbedGenerator:
    """扫描多个 shader 目录，生成 embed.hpp 内容"""

    def __init__(self, config: Dict[str, Any]) -> None:
        self.shader_dirs: List[Path] = config.get('shader_dirs', [Path('.').resolve()])
        self.output_file: Path = config['output_file']
        self.extensions: List[str] = config.get('extensions', ['.spv'])
        self.dry_run: bool = config.get('dry_run', False)
        self.verbose: bool = config.get('verbose', False)
        self.prefix: str = config.get('prefix', '../shader/')
        if self.prefix and not self.prefix.endswith('/'):
            self.prefix += '/'

        self.logger: logging.Logger = logging.getLogger('EmbedGenerator')
        if self.verbose:
            self.logger.setLevel(logging.DEBUG)
        else:
            self.logger.setLevel(logging.INFO)
        if not self.logger.handlers:
            handler: logging.Handler = logging.StreamHandler()
            handler.setFormatter(logging.Formatter('%(levelname)s: %(message)s'))
            self.logger.addHandler(handler)

    def collect_spv_files(self) -> List[Tuple[Path, Path]]:
        """
        递归收集所有 shader_dirs 下的 .spv 文件。
        返回 (shader_dir, file_path) 列表。
        """
        all_files: List[Tuple[Path, Path]] = []
        for shader_dir in self.shader_dirs:
            if not shader_dir.is_dir():
                self.logger.warning(f"目录不存在，跳过: {shader_dir}")
                continue
            for ext in self.extensions:
                pattern: str = f"*{ext}"
                for f in shader_dir.rglob(pattern):
                    all_files.append((shader_dir, f))
        all_files.sort(key=lambda x: str(x[1].relative_to(x[0])))
        self.logger.debug(f"找到 {len(all_files)} 个文件")
        return all_files

    def generate_definitions(
        self, files: List[Tuple[Path, Path]]
    ) -> Tuple[List[str], List[str], List[str], List[Tuple[str, str]], Set[str]]:
        """
        遍历文件，生成宏、数组、span 定义。
        返回 (defines, arrays, spans, map_entries, used_names)
        """
        defines: List[str] = []
        arrays: List[str] = []
        spans: List[str] = []
        map_entries: List[Tuple[str, str]] = []
        used_names: Set[str] = set()

        for shader_dir, file_path in files:
            rel_path: Path = file_path.relative_to(shader_dir)
            rel_str: str = str(rel_path).replace('\\', '/')
            base: str = str(rel_path.with_suffix('')).replace('\\', '/')
            parts: List[str] = base.split('.')
            if len(parts) == 1:
                root: str = parts[0]
                ext: str = ''
            else:
                root = '.'.join(parts[:-1])
                ext = parts[-1]
            if not ext:
                root = base
                ext = 'spv'

            root = NamingConverter.sanitize_identifier(root)
            ext = NamingConverter.sanitize_identifier(ext) if ext else 'spv'

            macro_value: str = f"{self.prefix}{rel_str}"

            root_clean: str = root.replace('/', '_')
            root_snake: str = NamingConverter.camel_to_snake(root_clean)
            root_snake_upper: str = root_snake.upper()
            ext_upper: str = ext.upper()
            ext_lower: str = ext.lower()

            macro: str = f"{root_snake_upper}_{ext_upper}"
            var: str = f"{root_snake}_{ext_lower}"
            raw_var: str = f"raw_{var}"

            macro = NamingConverter.sanitize_identifier(macro).upper()
            var = NamingConverter.sanitize_identifier(var).lower()
            raw_var = NamingConverter.sanitize_identifier(raw_var).lower()

            lower_macro: str = macro.lower()
            if lower_macro in used_names:
                self.logger.error(f"命名冲突: 宏 '{macro}' 与已有名称冲突（来自 {rel_str}）")
                sys.exit(1)
            if var in used_names:
                self.logger.error(f"命名冲突: 变量 '{var}' 与已有名称冲突（来自 {rel_str}）")
                sys.exit(1)
            used_names.add(lower_macro)
            used_names.add(var)

            key: str = base
            map_entries.append((key, var))

            defines.append(f"#define {macro} \"{macro_value}\"")
            arrays.append(
                f"    alignas(4) static constexpr u8 {raw_var}[] = {{\n"
                f"        #embed {macro}\n"
                f"    }};"
            )
            spans.append(
                f"    constexpr cu8span<sizeof({raw_var})>\n"
                f"        {var}({raw_var});"
            )

        return defines, arrays, spans, map_entries, used_names

    def generate_content(
        self,
        defines: List[str],
        arrays: List[str],
        spans: List[str],
        map_entries: List[Tuple[str, str]]
    ) -> List[str]:
        n: int = len(map_entries)
        lines: List[str] = []

        # ---------- 文件头 ----------
        lines.append("// ============================================================\n")
        lines.append("// 此文件由 embed_to_hpp.py 自动生成，请勿手动编辑。\n")
        lines.append(f"// 生成时间: {datetime.now().isoformat()}\n")
        lines.append("// 若要更新，请重新运行该脚本。\n")
        lines.append("// ============================================================\n")
        lines.append("\n")
        lines.append("#pragma once\n")
        lines.append("\n")
        lines.append("#include <cstddef>\n")
        lines.append("#include <cstdint>\n")
        lines.append("#include <span>\n")
        lines.append("#include \"frozen/unordered_map.h\"\n")
        lines.append("#include \"frozen/string.h\"\n")
        lines.append("\n")
        lines.append("using u8 = std::uint8_t;\n")
        lines.append("\n")

        # ---------- 命名空间 ----------
        lines.append("namespace embed {\n")
        lines.append("\n")
        lines.append("    template <size_t Extent = std::dynamic_extent>\n")
        lines.append("    using cu8span = std::span<const u8, Extent>;\n")
        lines.append("\n")
        lines.append(
            f"    using SpvsMap = frozen::unordered_map<"
            f"frozen::string, const std::span<const u8>, {n}>;\n"
        )
        lines.append("\n")

        # ---------- 辅助函数：计算 shader 名称总大小 ----------
        lines.append("    constexpr auto calcu_total_size(const embed::SpvsMap &spvs) -> size_t {\n")
        lines.append("        size_t total_sz = 0;\n")
        lines.append("        for (auto it = spvs.begin(); it != spvs.end(); ++it) {\n")
        lines.append("            total_sz += it->second.size();\n")
        lines.append("        }\n")
        lines.append("        return total_sz;\n")
        lines.append("    }\n")
        lines.append("\n")

        # ---------- 宏定义 ----------
        for d in defines:
            lines.append(f"    {d}\n")
        lines.append("\n")

        # ---------- 数组 + span ----------
        for arr, spn in zip(arrays, spans):
            lines.append(f"{arr}\n")
            lines.append(f"{spn}\n")
            lines.append("\n")

        # ---------- 映射表 ----------
        if n > 0:
            map_init: str = ",\n        ".join(
                [f'{{"{key}", {var}}}' for key, var in map_entries]
            )
            lines.append(f"    constexpr SpvsMap spvs_map{{\n        {map_init}\n    }};\n")
        else:
            lines.append("    constexpr SpvsMap spvs_map{};\n")

        lines.append("\n")
        lines.append("    static constexpr size_t total_sz = calcu_total_size(spvs_map);\n")
        lines.append("\n")

        lines.append("}; // namespace embed\n")
        return lines

    def run(self) -> int:
        self.logger.info(f"扫描目录: {self.shader_dirs}")
        files: List[Tuple[Path, Path]] = self.collect_spv_files()
        if not files:
            self.logger.warning("未找到任何 .spv 文件，将生成空头文件")
            defines: List[str] = []
            arrays: List[str] = []
            spans: List[str] = []
            map_entries: List[Tuple[str, str]] = []
            _used_names: Set[str] = set()
        else:
            defines, arrays, spans, map_entries, _used_names = self.generate_definitions(files)

        content: List[str] = self.generate_content(defines, arrays, spans, map_entries)

        if self.dry_run:
            self.logger.info("模拟运行，将写入以下内容（预览前100行）：")
            for line in content[:100]:
                print(line, end='')
            if len(content) > 100:
                print("... (省略)")
            return len(files)

        self.output_file.parent.mkdir(parents=True, exist_ok=True)
        tmp_file: Path = self.output_file.with_suffix(self.output_file.suffix + ".tmp")
        try:
            with open(tmp_file, 'w', encoding='utf-8') as f:
                f.writelines(content)
            tmp_file.replace(self.output_file)
            self.logger.info(f"成功写入: {self.output_file}")
        except Exception as e:
            self.logger.error(f"写入失败: {e}")
            if tmp_file.exists():
                tmp_file.unlink()
            sys.exit(1)

        return len(files)


# ---------- 命令行接口 ----------
def main() -> None:
    parser = argparse.ArgumentParser(
        description="自动生成 Shader 嵌入头文件 embed.hpp（支持多个目录）"
    )
    # ---------- 输入：包含 .spv 的目录（一个或多个） ----------
    parser.add_argument(
        '--shader-dirs',
        '--shader-dir',
        nargs='+',
        type=Path,
        default=[Path(__file__).parent],
        dest='shader_dirs',
        help="包含 .spv 文件的目录列表，用空格分隔（默认: 脚本所在目录）"
    )
    # ---------- 输出：指定 .hpp 所在目录 ----------
    parser.add_argument(
        '--output-dir',
        type=Path,
        default=Path(__file__).parent.parent,
        help="输出 .hpp 文件的目录（默认: 脚本所在目录的父目录）"
    )
    # ---------- 输出：指定 .hpp 文件名 ----------
    parser.add_argument(
        '--output-name',
        default="embed.hpp",
        help="输出的头文件名（默认: embed.hpp）"
    )
    # ---------- 输出：直接指定完整路径（可选，优先级最高） ----------
    parser.add_argument(
        '--output',
        type=Path,
        default=None,
        help="直接指定输出 .hpp 的完整路径；若指定则覆盖 --output-dir 与 --output-name"
    )
    parser.add_argument(
        '--prefix',
        default="../shader/",
        help="宏定义中路径前缀，如 '../shader/'（默认）"
    )
    parser.add_argument(
        '--extensions',
        nargs='+',
        default=['.spv'],
        help="要处理的扩展名（默认: .spv），可指定多个，如 .spv .spirv"
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help="只显示处理结果，不写入文件"
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help="输出详细日志"
    )
    args: argparse.Namespace = parser.parse_args()

    shader_dirs: List[Path] = [d.resolve() for d in args.shader_dirs]

    # ---------- 计算输出文件路径 ----------
    if args.output is not None:
        output_file: Path = args.output.resolve()
    else:
        output_dir: Path = args.output_dir.resolve()
        output_file = output_dir / args.output_name

    config: Dict[str, Any] = {
        'shader_dirs': shader_dirs,
        'output_file': output_file,
        'prefix': args.prefix,
        'extensions': args.extensions,
        'dry_run': args.dry_run,
        'verbose': args.verbose,
    }

    generator: EmbedGenerator = EmbedGenerator(config)
    count: int = generator.run()

    if not args.dry_run:
        print(f" 完成，共处理 {count} 个文件。输出: {output_file}")


if __name__ == "__main__":
    main()