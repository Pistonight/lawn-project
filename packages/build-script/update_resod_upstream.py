
import sys
import subprocess
import traceback
from pathlib import Path

from src import _upstream, _common, _fmt
from src.srcdrop import _libfiles, _frameworkfiles, _buildfiles, _assetfiles

def main():
    root = _common.get_root_root()

    is_continue = "--continue" in sys.argv

    if not is_continue:
        _cherry_pick_update(root)
        return

    subprocess.check_call(["git", "-C", root, "cherry-pick", "--continue", "--no-edit"])
    run_transform()
    _after_resolve_update(root)

def _cherry_pick_update(root: Path):
    if not _common.is_repo_clean(root):
        print(f"{_fmt.YELLOW}>>> repo contains changes, must be clean to take src drop{_fmt.RESET}")
        exit(1)
    run_transform()
    if not _common.is_repo_clean(root):
        print(f"{_fmt.YELLOW}>>> repo contains transformation changes, code must be formatted to take src drop{_fmt.RESET}")
        exit(1)

    config = _common.get_upstream_config()
    current_commit = config["current"]
    update_commit = config["update"]
    if current_commit == update_commit:
        print(f"{_fmt.YELLOW}>>> update commit is the same as current commit, please update config.json{_fmt.RESET}")
        exit(2)

    good_commit = _common.git_head_hash(root);

    # mark the starting point
    subprocess.call(["git", "branch", "-D", "srcdrop_/start"], text=True, stderr=subprocess.DEVNULL)
    subprocess.check_call(["git", "checkout", "-b", "srcdrop_/start"])
    # work on a temp branch from starting point
    subprocess.call(["git", "branch", "-D", "srcdrop_/temp"], text=True, stderr=subprocess.DEVNULL)
    subprocess.check_call(["git", "checkout", "-b", "srcdrop_/temp"])

    _upstream.ensure_repo(current_commit)
    try:
        # copy upstream (unpatched) to our project, which undoes our patches
        _copy_files()
        print(f"{_fmt.PINK}==> generating commit with our patches{_fmt.RESET}")
        # commit on temp branch and revert
        _common.git_commit_all(root, "temp: undo our changes");
        subprocess.check_call(["git", "-C", root, "revert", "--no-edit", "HEAD"])
        # this is the hash for our patches
        our_patch_commit = _common.git_head_hash(root);
        # mark this commit
        subprocess.call(["git", "branch", "-D", "srcdrop_/temp_our_patch"], text=True, stderr=subprocess.DEVNULL)
        subprocess.check_call(["git", "checkout", "-b", "srcdrop_/temp_our_patch"])
        # reset to good state
        subprocess.check_call(["git", "-C", root, "reset", "--hard", good_commit])
        # mark as new starting point
        subprocess.call(["git", "branch", "-D", "srcdrop_/temp"], text=True, stderr=subprocess.DEVNULL)
        subprocess.check_call(["git", "checkout", "-b", "srcdrop_/temp"])
        # update to the next patch
        print(f"{_fmt.PINK}==> upgrading framework{_fmt.RESET}")
        _upstream.ensure_repo(update_commit)
        _copy_files()
        _common.git_commit_all(root, "framework: upgrade ResoddedFramework");
    except Exception as e:
        print(e)
        traceback.print_exc()
        print(f"{_fmt.RED}>>> operation failed, reverting changes{_fmt.RESET}")
        _common.git_reset_repo(root)
        subprocess.check_call(["git", "-C", root, "reset", "--hard", good_commit])
        exit(1)
    # apply our patch
    print(f"{_fmt.PINK}==> applying our patch{_fmt.RESET}")
    cherry_pick_result = subprocess.call(["git", "-C", root, "cherry-pick", our_patch_commit])
    if cherry_pick_result == 0:
        print(f"{_fmt.GREEN}>>> no conflicts!{_fmt.RESET}")
        _after_resolve_update(root)
        return
    print(f"{_fmt.YELLOW}>>> there are conflicts with our patch")
    print(">>> run `git status` and resolve each conflict, use `git add` to mark the file as resolved")
    print(">>> then run the script again (when stil in cherry-picking state)")
    print(f">>>   DON'T RUN cherry-pick --continue manually!{_fmt.RESET}")
    exit(1)

def _after_resolve_update(root: Path):
    _common.git_commit_all(root, "framework: apply our patches");
    subprocess.check_call(["git", "checkout", "srcdrop_/start"])
    subprocess.check_call(["git", "merge", "--squash", "srcdrop_/temp"])

    config = _common.get_upstream_config()
    config["current"] = config["update"]
    _common.save_upstream_config(config)

    _common.git_commit_all(root, "framework: upgrade ResoddedFramework with our patches");
    subprocess.call(["git", "branch", "-D", "srcdrop_/done"])
    subprocess.check_call(["git", "checkout", "-b", "srcdrop_/done"])
    # clean up temp branches
    subprocess.call(["git", "branch", "-D", "srcdrop_/temp"])
    subprocess.call(["git", "branch", "-D", "srcdrop_/start"])
    subprocess.call(["git", "branch", "-D", "srcdrop_/temp_our_patch"])
    print(f"{_fmt.YELLOW}>>> done! to apply the changes to another branch:{_fmt.RESET}")
    print("   - git checkout <old branch>")
    print("   - git merge srcdrop_/done --ff-only")


def _copy_files():
    _libfiles.copy_files()
    _frameworkfiles.copy_files()
    _buildfiles.copy_files()
    _assetfiles.copy_files()
    
def run_transform():
    _libfiles.transform()
    _frameworkfiles.transform()
    _buildfiles.transform()
    _assetfiles.transform()
