import subprocess

from . import _common, _fmt

def ensure_repo(commit: str):
    repo_path = _common.get_upstream_root();
    clean_clone = True
    print(f"{_fmt.PINK}==> checking out ResoddedFramework{_fmt.RESET}")
    if repo_path.exists():
        clean_clone = False
        is_clean = True
        try:
            is_clean = _common.is_repo_clean(repo_path)
        except:
            clean_clone = True
        if not is_clean:
            raise Exception("local ResoddedFramework repo must be clean to continue")

        if not clean_clone:
            try:
                subprocess.check_call(["git", "-C", repo_path, "fetch"])
                subprocess.check_call(["git", "-C", repo_path, "reset", "--hard", commit])
                print(f"{_fmt.PINK}==> checked out ResoddedFramework " + commit + _fmt.RESET)
            except:
                clean_clone = True

    if not clean_clone:
        return
    print(">>> failed to access local repo, will clean clone")
    config = _common.get_upstream_config()
    repo = config["remote"]
    _common.rm_rf(repo_path)
    root_path = _common.get_upstreampkg_root()
    subprocess.check_call([ "git", "-C", root_path, "clone", repo, repo_path ])
    subprocess.check_call([ "git", "-C", repo_path, "config", "advice.detachedHead", "false" ])
    subprocess.check_call([ "git", "-C", repo_path, "checkout", commit ])

    print(f"{_fmt.PINK}==> cloned ResoddedFramework " + commit + _fmt.RESET)


