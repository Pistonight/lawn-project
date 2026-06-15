import sys
import subprocess
from pathlib import Path

import _upstream
import _common
import _libfiles
import _appfiles
import _buildfiles

def main():
    root = _common.get_root_root()

    is_continue = "--continue" in sys.argv

    if not is_continue:
        merge_update(root)
        return
    subprocess.check_call(["git", "-C", root, "cherry-pick", "--continue", "--no-edit"])
    fix_files()
    after_resolve_update(root)

def merge_update(root: Path):
    if not _common.is_repo_clean(root):
        print(">>> repo contains changes, must be clean to take src drop")
        exit(1)
    run_transform()
    if not _common.is_repo_clean(root):
        print(">>> repo contains formatting changes, code must be formatted to take src drop")
        exit(1)
    good_commit = _common.git_head_hash(root);

    subprocess.call(["git", "branch", "-D", "srcdrop_/start"])
    subprocess.check_call(["git", "checkout", "-b", "srcdrop_/start"])

    commit_file = _common.get_srcdrop_root() / "CURRENT_COMMIT"
    current_commit = commit_file.read_text().strip()

    _upstream.ensure_repo(current_commit)
    try:
        # copy upstream (unpatched) to our project, which undoes our patches
        copy_files()
        print("==> generating commit with our patches")
        # commit on temp branch and revert
        _common.git_commit_all(root, "temp: undo our changes");
        subprocess.check_call(["git", "-C", root, "revert", "--no-edit", "HEAD"])
        # this is the hash for our patches
        our_patch_commit = _common.git_head_hash(root);
        # mark this commit
        subprocess.call(["git", "branch", "-D", "srcdrop_/temp_our_patch"])
        subprocess.check_call(["git", "checkout", "-b", "srcdrop_/temp_our_patch"])
        # reset to good state
        subprocess.check_call(["git", "-C", root, "reset", "--hard", good_commit])
        # mark as new starting point
        subprocess.call(["git", "branch", "-D", "srcdrop_/temp"])
        subprocess.check_call(["git", "checkout", "-b", "srcdrop_/temp"])
        # update to the next patch
        print("==> upgrading framework")
        _upstream.ensure_repo_next()
        copy_files()
        _common.git_commit_all(root, "framework: upgrade ResoddedFramework");
    except:
        print(">>> operation failed, reverting changes")
        _common.git_reset_repo(root)
        subprocess.check_call(["git", "-C", root, "reset", "--hard", good_commit])
        exit(1)
    # apply our patch
    print("==> applying our patch")
    cherry_pick_result = subprocess.call(["git", "-C", root, "cherry-pick", our_patch_commit])
    if cherry_pick_result == 0:
        print(">>> no conflicts!")
        after_resolve_update(root)
        return
    print(">>> there are conflicts with our patch")
    print(">>> run `git status` and resolve each conflict, use `git add` to mark the file as resolved")
    print(">>> then run the script again (when stil in cherry-picking state)")
    print(">>>   DON'T RUN cherry-pick --continue manually!")
    exit(1)

def after_resolve_update(root: Path):
    _common.git_commit_all(root, "framework: apply our patches");
    subprocess.check_call(["git", "checkout", "srcdrop_/start"])
    subprocess.check_call(["git", "merge", "--squash", "srcdrop_/temp"])

    commit_file = _common.get_srcdrop_root() / "CURRENT_COMMIT"
    commit_file.write_text(_upstream.UPSTREAM_COMMIT.strip() + "\n")

    _common.git_commit_all(root, "framework: upgrade ResoddedFramework with our patches");
    subprocess.call(["git", "branch", "-D", "srcdrop_/done"])
    subprocess.check_call(["git", "checkout", "-b", "srcdrop_/done"])
    # clean up temp branches
    subprocess.call(["git", "branch", "-D", "srcdrop_/temp"])
    subprocess.call(["git", "branch", "-D", "srcdrop_/start"])
    subprocess.call(["git", "branch", "-D", "srcdrop_/temp_our_patch"])
    print(">>> done! to apply the changes to another branch:")
    print(">>> - git checkout <old branch>")
    print(">>> - git merge srcdrop_/done --ff-only")


def copy_files():
    _libfiles.copy_files()
    _appfiles.copy_files()
    _buildfiles.copy_files()
    
def fix_files():
    _libfiles.run_fix()
    _appfiles.run_fix()
    _buildfiles.run_fix()

def run_transform():
    _libfiles.transform()
    _appfiles.transform()
    _buildfiles.transform()

if __name__ == "__main__":
    main()
