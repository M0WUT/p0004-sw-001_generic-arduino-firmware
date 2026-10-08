from datetime import datetime, timezone
from pathlib import Path
import subprocess


def git(repo_dir, *args):
    return subprocess.check_output(
        ["git", "-C", str(repo_dir), *args],
        text=True,
        stderr=subprocess.DEVNULL,
    ).strip()


def generate_version_header(env):
    repo_dir = Path(env["PROJECT_DIR"]).resolve()
    output = repo_dir / "include" / "version.h"

    commit_hash = git(repo_dir, "rev-parse", "--short", "HEAD")

    # Try to find the latest tag.
    try:
        tag = git(repo_dir, "describe", "--tags", "--abbrev=0")
    except subprocess.CalledProcessError:
        tag = None

    if tag is None:
        # No tags in the repository.
        version = commit_hash
    else:
        commits_since_tag = int(git(repo_dir, "rev-list", "--count", f"{tag}..HEAD"))

        if commits_since_tag > 0:
            version = f"{tag}_dev{commit_hash}"
        else:
            version = tag

    # Add _dirty if there are staged, unstaged, or untracked changes.
    if git(repo_dir, "status", "--porcelain"):
        version += "_dirty"

    build_timestamp = (
        datetime.now(timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z")
    )

    output.parent.mkdir(parents=True, exist_ok=True)

    output.write_text(
        f'#define VERSION_STRING "{version}"\n'
        f'#define BUILD_TIMESTAMP "{build_timestamp}"\n',
        encoding="utf-8",
    )


Import("env")
generate_version_header(env)
