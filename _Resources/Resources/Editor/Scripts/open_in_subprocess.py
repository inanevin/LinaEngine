import subprocess
import argparse

def main():
    global parser
    parser = argparse.ArgumentParser()
    global args
    args = parse_args()
    p = subprocess.Popen([args.app, args.filename])
    
def parse_args():
    parser.add_argument("--app", required=True, dest="app", default=False, help="App to use while opening the file.")
    parser.add_argument("--filename", required=True, dest="filename", default=False, help="File to open.")
    parsed_args = parser.parse_args()
    return parsed_args

if __name__ == "__main__":
    main()
