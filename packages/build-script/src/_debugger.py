import os
import subprocess
from pathlib import Path

from . import _fmt

def launch_debugger_project(cmake_dir: Path, exe: Path):
    devenv = _find_devenv()
    if not devenv:
        exit(1)
    vcxproj = _make_vcxproj(cmake_dir, exe)
    if not vcxproj:
        exit(2)
    vcxproj_path = cmake_dir / "debugger.vcxproj"
    vcxproj_path.write_bytes(vcxproj.encode("utf-8"))
    subprocess.check_call([
        "powershell", "-c",
        f'start-process \'{devenv}\' \'{str(vcxproj_path)}\''])
    print(f"==> launched generated debugger project")


def _find_devenv() -> Path | None:
    systemdrive = _find_systemdrive()
    devenv = _find_devenv_in_programs(f"{systemdrive}\\Program Files")
    if not devenv:
        print(f"{_fmt.YELLOW}>>> cannot find devenv in Program Files, trying x86{_fmt.RESET}")
        devenv = _find_devenv_in_programs(f"{systemdrive}\\Program Files (x86)")
    if not devenv:
        print(f"{_fmt.RED}>>> cannot find devenv!{_fmt.RESET}")
        return None
    devenv_path, name = devenv
    print(f"==> found {name}")
    return devenv_path

def _find_devenv_in_programs(path: str) -> tuple[Path, str] | None:
    microsoft_visual_studio = Path(path) / "Microsoft Visual Studio"
    if not microsoft_visual_studio.exists():
        return None
    any_version = None
    max_version = 0
    for version in os.listdir(microsoft_visual_studio):
        any_version = version
        try:
            this_version = int(version)
            max_version = max(this_version, max_version)
        except ValueError:
            pass
    selected_version = any_version
    if max_version:
        selected_version = max_version
    if not selected_version:
        return None
    ms_visual_studio_year = microsoft_visual_studio / str(selected_version)
    for license in ["Enterprise", "Professional", "Community"]:
        devenv = ms_visual_studio_year / license / "Common7" / "IDE" / "devenv.exe";
        if devenv.is_file():
            return devenv, f"Visual Studio {selected_version} {license}"

    return None


def _find_systemdrive():
    systemdrive = os.environ["systemdrive"]
    if not systemdrive:
        print(f"{_fmt.YELLOW}>>> cannot determine system drive, assuming C:{_fmt.RESET}")
        systemdrive = "C:"
    return systemdrive

def _make_vcxproj(cmake_dir: Path, exe: Path) -> str | None:
    cwd = exe.parent
    cmake_proj = cmake_dir / "ZERO_CHECK.vcxproj"
    if not cmake_proj.exists():
        print(f"{_fmt.RED}>>> cannot determine project architecture and toolset{_fmt.RESET}")
        print(f"{_fmt.RED}>>> cmake project for msvc not found!!{_fmt.RESET}")
        return None
    platform = None
    toolset = None
    for line in cmake_proj.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if platform is None and line.startswith("<Platform>") and line.endswith("</Platform>"):
            l = len("<Platform>")
            platform = line[l:-(l+1)]
        elif toolset is None and line.startswith("<PlatformToolset>") and line.endswith("</PlatformToolset>"):
            l = len("<PlatformToolset>")
            toolset = line[l:-(l+1)]
    if toolset is None:
        print(f"{_fmt.RED}>>> failed to determine toolset from cmake project{_fmt.RESET}")
        return None
    if platform is None:
        print(f"{_fmt.YELLOW}==> failed to determine platform, assuming x64{_fmt.RESET}")
        platform = "x64"
    else:
        if platform == "Win32":
            print(f"==> platorm: Win32 (x86), toolset: {toolset}")
        else:
            print(f"==> platorm: x64, toolset: {toolset}")

    project_name = os.path.basename(exe).replace(".", "_");

    condition_string = f'"\'$(Configuration)|$(Platform)\'==\'Debug|{platform}\'"'

    lines = [
        '<?xml version="1.0" encoding="utf-8"?>',
        '<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">',
        '  <PropertyGroup><PreferredToolArchitecture>x64</PreferredToolArchitecture></PropertyGroup>',
        '  <ItemGroup Label="ProjectConfigurations">',
       f'    <ProjectConfiguration Include="Debug|{platform}">',
        '      <Configuration>Debug</Configuration>',
       f'      <Platform>{platform}</Platform>',
        '    </ProjectConfiguration>',
        '  </ItemGroup>',
        '  <PropertyGroup Label="Globals">',
        '    <ProjectGuid>{f8d0330b-7ada-4d93-a131-14fd1e3e0530}</ProjectGuid>',
        '    <Keyword>Win32Proj</Keyword>',
        '    <RootNamespace>DebuggerLauncher</RootNamespace>',
       f'    <ProjectName>{project_name}</ProjectName>',
        '  </PropertyGroup>',
        '  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />',
       f'  <PropertyGroup Condition={condition_string} Label="Configuration">',
        '    <ConfigurationType>Makefile</ConfigurationType>',
        '    <UseDebugLibraries>true</UseDebugLibraries>',
       f'    <PlatformToolset>{toolset}</PlatformToolset>',
        '  </PropertyGroup>',
        '  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />',
        '  <ImportGroup Label="ExtensionSettings" />',
        '  <ImportGroup Label="Shared" />',
        '  <ImportGroup Label="PropertySheets" />',
        '  <PropertyGroup Label="UserMacros" />',
       f'  <PropertyGroup Condition={condition_string}>',
        '    <NMakeBuildCommandLine></NMakeBuildCommandLine>',
        '    <NMakeReBuildCommandLine></NMakeReBuildCommandLine>',
        '    <NMakeCleanCommandLine></NMakeCleanCommandLine>',
       f'    <NMakeOutput>{exe}</NMakeOutput>',
       f'    <LocalDebuggerCommand>{exe}</LocalDebuggerCommand>',
       f'    <LocalDebuggerWorkingDirectory>{cwd}</LocalDebuggerWorkingDirectory>',
        '    <LocalDebuggerCommandArguments></LocalDebuggerCommandArguments>',
        '    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>',
        '  </PropertyGroup>',
        '  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />',
        '</Project>'
    ]

    return '\n'.join(lines)+'\n'
