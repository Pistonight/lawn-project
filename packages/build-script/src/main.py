import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

def main():
    if sys.argv[0].endswith("main.py"):
        argv = sys.argv[1:]
    else:
        argv = sys.argv
    if len(argv) < 1:
        print(">>> usage: main.py PROJECT TASK ARGS..")
    match argv[0]:
        case "lawn":
            import lawn
            return lawn.main(argv[1:])
        case "assets":
            import assets
            return assets.main(argv[1:])
        case "upstream":
            import upstream
            return upstream.main(argv[1:])

    print(f">>> unknown project {argv[0]}")
    return 64


if __name__ == "__main__":
    exit(main())
