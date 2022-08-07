import subprocess
import argparse

def main():
    global parser
    parser = argparse.ArgumentParser()
    global args
    args = parse_args()
    callArgs = args.glslangPath + " " + args.target
    p = subprocess.Popen(callArgs, stdout=FNULL, stderr=FNULL, shell=False)
    
def parse_args():
    parser.add_argument("--glslangPath", required=True, dest="glslangPath", default=False, help="Compiler executable.")
    parser.add_argument("--target", required=True, dest="target", default=False, help="Shader to compile.")
    parsed_args = parser.parse_args()
    return parsed_args

if __name__ == "__main__":
    main()
