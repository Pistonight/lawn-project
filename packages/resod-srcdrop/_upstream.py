import subprocess

import _common

# using our fork which contains patches before they are merged upstream
UPSTREAM_REPO = "https://github.com/Pistonight/ResoddedFramework"
# This is the commit we want to upgrade to
UPSTREAM_COMMIT = "6e082da7ec407f31a583353fa48f57d615c8a01a"
# UPSTREAM_COMMIT = "aadf5266f9c22ce70426cd6a1d2a78c8a654b006"

UPSTREAM_LIBS = [
    # -lib
    "ImageLib", "PakLib",
    # -framework
    "Lawn", "Sexy.TodLib", "SexyAppFramework",
]

def ensure_repo_next():
    ensure_repo(UPSTREAM_COMMIT)

def ensure_repo(commit: str):
    repo_path = _common.get_framework_root();
    clean_clone = True
    print("==> checking out ResoddedFramework")
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
                subprocess.check_call(["git", "-C", repo_path, "reset", "--hard", commit])
            except:
                clean_clone = True

    if not clean_clone:
        return
    print(">>> failed to access local repo, will clean clone")
    _common.rm_rf(repo_path)
    root_path = _common.get_srcdrop_root()
    subprocess.check_call([ "git", "-C", root_path, "clone", UPSTREAM_REPO, repo_path ])
    subprocess.check_call([ "git", "-C", repo_path, "config", "advice.detachedHead", "false" ])
    subprocess.check_call([ "git", "-C", repo_path, "checkout", commit ])

    print("==> checked out ResoddedFramework " + commit)


