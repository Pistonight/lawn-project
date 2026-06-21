import shutil
import subprocess
import json
from pathlib import Path

def get_root_root() -> Path:
    return get_packages_root().parent

def get_packages_root() -> Path:
    return Path(__file__).parent.parent.parent

def get_upstreampkg_root() -> Path:
    return get_packages_root() / "resod-upstream"

def get_upstream_root() -> Path:
    return get_upstreampkg_root() / "ResoddedFramework"

def get_upstream_config_path():
    return get_upstreampkg_root() / "config.json"

def get_upstream_config():
    return json.loads(get_upstream_config_path().read_text())

def save_upstream_config(config):
    get_upstream_config_path().write_text(
        json.dumps(config, indent=4) + "\n", encoding="utf8", newline="\n"
    )
    
def rm_rf(d: Path | str):
    if Path(d).exists():
        shutil.rmtree(d)
    
def is_src_file(s: str) -> bool:
    return s.endswith(".c") or s.endswith(".cpp")

def is_header_file(s: str) -> bool:
    return (s.endswith(".h")
        or s.endswith(".hpp")
        or s.endswith(".h.in")
        or s.endswith(".inc"))

def is_repo_clean(repo: Path | str) -> bool:
    status = subprocess.check_output(
        [ "git", "status", "--porcelain" ], cwd=repo, text=True
    )
    if status.strip():
        print(">>> repo not clean")
        print(status)
        return False
    return True

def git_reset_repo(repo: Path | str):
    subprocess.check_call(
        [ "git", "checkout", "." ], cwd=repo
    )

def git_commit_all(repo: Path | str, message: str):
    subprocess.check_call([ "git", "add", "." ], cwd=repo)
    subprocess.check_call([ "git", "commit", "--allow-empty", "-m", message ], cwd=repo)

def git_head_hash(repo: Path | str) -> str:
    return subprocess.check_output(
        [ "git", "rev-parse", "HEAD" ], cwd=repo, text=True
    ).strip()
