import subprocess
import sys
import re
import os
import glob

def find_uproject():
    """Find the .uproject file in the current directory"""
    uproject_files = glob.glob("*.uproject")
    if not uproject_files:
        return None
    return uproject_files[0]

def find_unreal_engine():
    """Find Unreal Engine installation"""
    common_paths = [
        r"C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.3\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.2\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.1\Engine\Build\BatchFiles\Build.bat",
        r"C:\Program Files\Epic Games\UE_5.0\Engine\Build\BatchFiles\Build.bat",
    ]
    
    for path in common_paths:
        if os.path.exists(path):
            return os.path.dirname(os.path.dirname(path))
    
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\EpicGames\Unreal Engine")
        value, _ = winreg.QueryValueEx(key, "INSTALLDIR")
        batch_files_path = os.path.join(value, "Engine", "Build", "BatchFiles")
        if os.path.exists(batch_files_path):
            return batch_files_path
    except:
        pass
    
    return None

def generate_project_files(uproject_path):
    """Generate Visual Studio solution files"""
    print("\n=== Regenerating Visual Studio Solution Files ===")
    
    engine_path = find_unreal_engine()
    if not engine_path:
        print("WARNING: Could not find Unreal Engine installation!")
        return False
    
    generate_script = os.path.join(engine_path, "GenerateProjectFiles.bat")
    
    if not os.path.exists(generate_script):
        print(f"WARNING: GenerateProjectFiles.bat not found")
        return False
    
    try:
        abs_uproject_path = os.path.abspath(uproject_path)
        subprocess.run(
            [generate_script, f'-project="{abs_uproject_path}"', '-game', '-engine'],
            check=True,
            cwd=os.getcwd()
        )
        print("✓ Solution files regenerated successfully!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"WARNING: Error generating solution files: {e}")
        return False

def build_project(uproject_path):
    """Build the Unreal project to verify code compiles"""
    print("\n=== Building Project for Verification ===")
    
    engine_path = find_unreal_engine()
    if not engine_path:
        print("ERROR: Could not find Unreal Engine installation!")
        return False
    
    build_script = os.path.join(engine_path, "Build.bat")
    
    if not os.path.exists(build_script):
        print(f"ERROR: Build.bat not found")
        return False
    
    project_name = os.path.splitext(os.path.basename(uproject_path))[0]
    abs_uproject_path = os.path.abspath(uproject_path)
    
    print(f"Building project: {project_name}")
    print("This may take several minutes...")
    
    try:
        subprocess.run(
            [
                build_script,
                f"{project_name}Editor",
                "Win64",
                "Development",
                f'"{abs_uproject_path}"'
            ],
            check=True,
            cwd=os.getcwd()
        )
        print("✓ Project built successfully!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ BUILD FAILED: {e}")
        return False

def get_current_branch():
    result = subprocess.run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"],
        capture_output=True,
        text=True,
        check=True
    )
    return result.stdout.strip()

def has_changes():
    result = subprocess.run(
        ["git", "status", "--porcelain"],
        capture_output=True,
        text=True
    )
    return bool(result.stdout.strip())

def get_staged_files():
    result = subprocess.run(
        ["git", "diff", "--cached", "--name-only"],
        capture_output=True,
        text=True
    )
    return result.stdout.strip().split('\n') if result.stdout.strip() else []

def check_for_binary_conflicts(branch_name):
    """Check if the same binary files were changed on both branches since their common ancestor"""
    try:
        subprocess.run(["git", "fetch", "origin", "main"], check=True)

        # Find the common ancestor of this branch and origin/main
        merge_base_result = subprocess.run(
            ["git", "merge-base", "origin/main", branch_name],
            capture_output=True,
            text=True,
            check=True
        )
        merge_base = merge_base_result.stdout.strip()

        binary_extensions = ['.uasset', '.umap', '.uexp', '.ubulk']

        # Files changed on this branch since the merge base
        branch_result = subprocess.run(
            ["git", "diff", "--name-only", merge_base, branch_name],
            capture_output=True,
            text=True
        )
        branch_binaries = {
            f for f in branch_result.stdout.splitlines()
            if any(f.endswith(ext) for ext in binary_extensions)
        }

        if not branch_binaries:
            return []

        # Files changed on main since the merge base
        main_result = subprocess.run(
            ["git", "diff", "--name-only", merge_base, "origin/main"],
            capture_output=True,
            text=True
        )
        main_binaries = {
            f for f in main_result.stdout.splitlines()
            if any(f.endswith(ext) for ext in binary_extensions)
        }

        # Only files modified on BOTH sides are genuine conflicts
        return list(branch_binaries & main_binaries)

    except subprocess.CalledProcessError:
        return []

def has_code_changes():
    """Check if there are any C++ code changes"""
    result = subprocess.run(
        ["git", "diff", "--cached", "--name-only"],
        capture_output=True,
        text=True
    )
    
    changed_files = result.stdout.strip().split('\n')
    code_extensions = ['.cpp', '.h', '.cs', '.build.cs', '.target.cs']
    
    return any(any(f.endswith(ext) for ext in code_extensions) for f in changed_files)

def main():
    print("=== Unreal Engine Work Completion Tool ===\n")
    
    # Find .uproject
    uproject = find_uproject()
    if uproject:
        print(f"Found project: {uproject}\n")
    
    # Get current branch
    current_branch = get_current_branch()
    
    if current_branch == "main":
        print("ERROR: You are on the main branch!")
        print("This tool should be run from your working branch.")
        input("Press Enter to exit")
        sys.exit(1)
    
    print(f"Current branch: {current_branch}\n")
    
    # Check for uncommitted changes
    if has_changes():
        print("You have uncommitted changes. Staging project files only...")
        
        subprocess.run(["git", "reset"], check=True)
        
        result = subprocess.run(
            ["git", "status", "--porcelain"],
            capture_output=True,
            text=True
        )
        
        # Filter files to exclude
        exclude_patterns = [
            'Binaries/',
            'Build/',
            'Intermediate/',
            'Saved/',
            'DerivedDataCache/',
            '.vs/',
            '*.log',
            '*.crash',
            '*.pdb',
            '*.obj',
            '*.exe',
            '*.dll',
            '*.sln',           # Exclude solution files
            '*.vcxproj',       # Exclude project files
            '*.vcxproj.filters',
            '*.vcxproj.user',
        ]
        
        for line in result.stdout.splitlines():
            if not line:
                continue
            
            file_path = line[3:]
            
            should_exclude = any(pattern in file_path or file_path.endswith(pattern.replace('*', '')) 
                                for pattern in exclude_patterns)
            
            if not should_exclude:
                print(f"Staging: {file_path}")
                subprocess.run(["git", "add", file_path])
        
        staged_files = get_staged_files()
        if not staged_files or (len(staged_files) == 1 and not staged_files[0]):
            print("\nNo project files to commit (only build artifacts/logs were changed).")
            input("Press Enter to exit")
            sys.exit(0)
        
        commit_msg = input("\nEnter commit message: ").strip()
        if not commit_msg:
            commit_msg = f"Work completed on {current_branch}"
        
        subprocess.run(["git", "commit", "-m", commit_msg], check=True)
        print("Changes committed successfully.\n")
    
    # Check if there are code changes that require building
    if uproject and has_code_changes():
        print("⚠️  Code changes detected!")
        build_choice = input("Build project to verify code compiles? (recommended - y/n): ").strip().lower()
        
        if build_choice == 'y':
            # Regenerate solution files first
            generate_project_files(uproject)
            
            # Build project
            if not build_project(uproject):
                print("\n❌ BUILD FAILED!")
                print("Please fix compilation errors before pushing.")
                retry = input("\nDo you want to continue anyway? (not recommended - y/n): ").strip().lower()
                if retry != 'y':
                    print("Aborting. Fix the build errors and run this tool again.")
                    input("Press Enter to exit")
                    sys.exit(1)
    
    # Check for binary file conflicts
    print("\nChecking for potential conflicts with Unreal binary files...")
    conflicts = check_for_binary_conflicts(current_branch)
    
    if conflicts:
        print("\n⚠️  CONFLICT DETECTED ⚠️")
        print("The following Unreal binary files have conflicts with main:")
        for conflict in conflicts:
            print(f"  - {conflict}")
        print("\nPublishing branch to remote for manual resolution...")
        
        subprocess.run(["git", "push", "origin", current_branch], check=True)
        print(f"\n✓ Branch '{current_branch}' pushed to remote.")
        print("Please resolve conflicts manually and create a pull request.")
        
    else:
        print("No conflicts detected. Merging to main...\n")
        
        try:
            subprocess.run(["git", "checkout", "main"], check=True)
            subprocess.run(["git", "pull", "origin", "main"], check=True)
            subprocess.run(["git", "merge", current_branch, "--no-ff", "-m", f"Merge branch '{current_branch}'"], check=True)
            subprocess.run(["git", "push", "origin", "main"], check=True)
            
            print(f"✓ Changes merged to main and pushed successfully!")
            
            subprocess.run(["git", "branch", "-d", current_branch], check=True)
            print(f"✓ Local branch '{current_branch}' deleted.")
            
            # Only delete remote branch if it was published to origin
            remote_check = subprocess.run(
                ["git", "ls-remote", "--heads", "origin", current_branch],
                capture_output=True, text=True
            )
            if remote_check.stdout.strip():
                subprocess.run(["git", "push", "origin", "--delete", current_branch], check=True)
                print(f"✓ Remote branch '{current_branch}' deleted.")
            
        except subprocess.CalledProcessError as e:
            print(f"\n❌ Error during merge: {e}")
            print("Please resolve manually.")
            input("Press Enter to exit")
            sys.exit(1)
    
    print("\n=== Work completion finished ===")
    input("Press Enter to exit")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nOperation cancelled by user.")
        sys.exit(0)