#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
递归编译 .glsl 文件为 SPIR-V (.spv)。
使用 glslangValidator 工具，保持原始目录结构。
"""

from __future__ import annotations

import sys
import argparse
import logging
import subprocess
from pathlib import Path
from typing import List, Dict, Any


class GlslangCompiler:
    """使用 glslangValidator 编译 GLSL 文件"""

    def __init__(self, config: Dict[str, Any]) -> None:
        self.input_dir: Path = config.get('input_dir', Path('.').resolve())
        self.output_dir: Path = config['output_dir']  # 必须指定
        self.glslang_cmd: str = config.get('glslang_cmd', 'glslangValidator')
        self.extensions: List[str] = config.get('extensions', ['.glsl'])
        self.verbose: bool = config.get('verbose', False)
        self.dry_run: bool = config.get('dry_run', False)
        self.stop_on_error: bool = config.get('stop_on_error', False)
        self.overwrite: bool = config.get('overwrite', True)

        self.logger = self._setup_logger()

    def _setup_logger(self) -> logging.Logger:
        logger = logging.getLogger('GlslangCompiler')
        if self.verbose:
            logger.setLevel(logging.DEBUG)
        else:
            logger.setLevel(logging.INFO)
        if not logger.handlers:
            handler = logging.StreamHandler()
            handler.setFormatter(logging.Formatter('%(levelname)s: %(message)s'))
            logger.addHandler(handler)
        return logger

    def collect_glsl_files(self) -> List[Path]:
        """递归收集所有符合扩展名的文件"""
        files: List[Path] = []
        for ext in self.extensions:
            pattern: str = f"*{ext}"
            # 使用 rglob 递归搜索
            for f in self.input_dir.rglob(pattern):
                files.append(f)
        files.sort(key=lambda p: str(p.relative_to(self.input_dir)))
        self.logger.debug(f"找到 {len(files)} 个 .glsl 文件")
        return files

    def compile_file(self, input_file: Path, output_file: Path) -> bool:
        """编译单个文件，返回是否成功"""
        if not self.overwrite and output_file.exists():
            self.logger.debug(f"跳过已存在: {output_file}")
            return True

        if self.dry_run:
            self.logger.info(f"[模拟] 编译 {input_file} -> {output_file}")
            return True

        # 确保输出目录存在
        output_file.parent.mkdir(parents=True, exist_ok=True)

        cmd: List[str] = [
            self.glslang_cmd,
            '-V',           # Vulkan SPIR-V
            '-o', str(output_file),
            str(input_file)
        ]
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                check=False
            )
            if result.returncode != 0:
                self.logger.error(f"编译失败: {input_file}")
                self.logger.error(f"错误输出:\n{result.stderr}")
                return False
            else:
                self.logger.info(f"编译成功: {input_file} -> {output_file}")
                return True
        except FileNotFoundError:
            self.logger.error(f"找不到命令: {self.glslang_cmd}")
            self.logger.error("请确保 glslangValidator 已安装并在 PATH 中")
            sys.exit(1)
        except Exception as e:
            self.logger.error(f"编译时异常: {e}")
            return False

    def run(self) -> int:
        """执行编译流程，返回成功编译的文件数"""
        self.logger.info(f"输入目录: {self.input_dir}")
        self.logger.info(f"输出目录: {self.output_dir}")

        # 检查输入目录
        if not self.input_dir.is_dir():
            self.logger.error(f"输入目录不存在: {self.input_dir}")
            sys.exit(1)

        # 创建输出目录（如果不存在）
        self.output_dir.mkdir(parents=True, exist_ok=True)

        # 收集文件
        files: List[Path] = self.collect_glsl_files()
        if not files:
            self.logger.warning("未找到任何 .glsl 文件")
            return 0

        success_count: int = 0
        failed_files: List[Path] = []

        for input_file in files:
            # 计算相对路径（相对于 input_dir）
            rel_path: Path = input_file.relative_to(self.input_dir)
            # 替换扩展名为 .spv
            output_rel: Path = rel_path.with_suffix('.spv')
            output_file: Path = self.output_dir / output_rel

            # 编译
            ok = self.compile_file(input_file, output_file)
            if ok:
                success_count += 1
            else:
                failed_files.append(input_file)
                if self.stop_on_error:
                    self.logger.error("因错误而停止")
                    break

        # 输出统计
        self.logger.info(f"编译完成: 成功 {success_count}，失败 {len(failed_files)}")
        if failed_files:
            self.logger.warning("失败的文件:")
            for f in failed_files:
                self.logger.warning(f"  {f}")

        return success_count


def main() -> None:
    parser = argparse.ArgumentParser(
        description="递归编译 GLSL 文件为 SPIR-V"
    )
    parser.add_argument(
        '--input-dir',
        type=Path,
        default=Path('.').resolve(),
        help="输入目录（默认: 当前目录）"
    )
    parser.add_argument(
        '--output-dir',
        type=Path,
        required=True,
        help="输出目录（必须指定）"
    )
    parser.add_argument(
        '--glslang',
        default='glslangValidator',
        help="glslang 命令（默认: glslangValidator）"
    )
    parser.add_argument(
        '--extensions',
        nargs='+',
        default=['.glsl'],
        help="要处理的扩展名（默认: .glsl）"
    )
    parser.add_argument(
        '--stop-on-error',
        action='store_true',
        help="遇到编译错误时停止"
    )
    parser.add_argument(
        '--no-overwrite',
        action='store_true',
        help="不覆盖已存在的 .spv 文件"
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help="只显示将执行的操作，不实际编译"
    )
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help="输出详细日志"
    )
    args: argparse.Namespace = parser.parse_args()

    input_dir: Path = args.input_dir.resolve()
    output_dir: Path = args.output_dir.resolve()

    config: Dict[str, Any] = {
        'input_dir': input_dir,
        'output_dir': output_dir,
        'glslang_cmd': args.glslang,
        'extensions': args.extensions,
        'verbose': args.verbose,
        'dry_run': args.dry_run,
        'stop_on_error': args.stop_on_error,
        'overwrite': not args.no_overwrite,
    }

    compiler = GlslangCompiler(config)
    count: int = compiler.run()

    if not args.dry_run:
        print(f"✅ 编译完成，成功 {count} 个文件。")


if __name__ == "__main__":
    main()