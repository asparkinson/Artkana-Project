import subprocess
import sys
import re
import os
import glob

def find_uproject():
    """Find the .uproject file in the current directory"""
    uproject_files = glob.glob("*.uproject")
    if not uproject_files:
        print("ERROR: No .uproject file found in current directory!")
        return None
    if len(uproject_files) > 1:
        print("WARNING: Multiple .uproject files found. Using:", uproject_files[0])
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
            return os.path.dirname(path)  # Return Engine/Build/BatchFiles directory
    
    # Check registry for custom installation
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
    print("\n=== Generating Visual Studio Solution Files ===")

    batch_files_path = find_unreal_engine()
    if not batch_files_path:
        print("ERROR: Could not find Unreal Engine installation!")
        return False

    # UnrealBuildTool is at Engine/Binaries/DotNET/UnrealBuildTool/ (two levels up from BatchFiles)
    engine_dir = os.path.dirname(os.path.dirname(batch_files_path))
    ubt = os.path.join(engine_dir, "Binaries", "DotNET", "UnrealBuildTool", "UnrealBuildTool.exe")

    if not os.path.exists(ubt):
        print(f"ERROR: UnrealBuildTool.exe not found at {ubt}")
        return False

    abs_uproject_path = os.path.abspath(uproject_path)
    print(f"Generating solution for: {uproject_path}")

    try:
        subprocess.run(
            [ubt, "-ProjectFiles", f"-project={abs_uproject_path}", "-game"],
            check=True
        )
        print("✓ Solution files generated successfully!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error generating solution files: {e}")
        return False

def build_project(uproject_path):
    """Build the Unreal project"""
    print("\n=== Building Unreal Project ===")
    
    engine_path = find_unreal_engine()
    if not engine_path:
        print("ERROR: Could not find Unreal Engine installation!")
        return False
    
    # Path to Build.bat
    build_script = os.path.join(engine_path, "Build.bat")
    
    if not os.path.exists(build_script):
        print(f"ERROR: Build.bat not found at {build_script}")
        return False
    
    # Extract project name from .uproject file
    project_name = os.path.splitext(os.path.basename(uproject_path))[0]
    abs_uproject_path = os.path.abspath(uproject_path)
    
    print(f"Building project: {project_name}")
    print("This may take several minutes...")
    
    try:
        # Build Development Editor configuration
        subprocess.run(
            [
                build_script,
                f"{project_name}Editor",
                "Win64",
                "Development",
                abs_uproject_path,
                "-game"
            ],
            check=True,
            cwd=os.getcwd()
        )
        print("✓ Project built successfully!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error building project: {e}")
        print("You may need to build manually in Visual Studio.")
        return False

def check_for_self_update():
    """Check origin/main for a newer version of this script and update/restart if found."""
    script_name = "setup_branch_Version2.py"
    script_path = os.path.join(os.getcwd(), script_name)

    try:
        subprocess.run(["git", "fetch", "origin"], check=True, capture_output=True)

        remote_result = subprocess.run(
            ["git", "show", f"origin/main:{script_name}"],
            capture_output=True, text=True, check=True
        )
        remote_content = remote_result.stdout

        if not os.path.exists(script_path):
            return

        with open(script_path, 'r', encoding='utf-8') as f:
            local_content = f.read()

        if remote_content == local_content:
            return

        print("\u26a1 A newer version of this tool is available on origin/main.")

        if getattr(sys, 'frozen', False):
            _hot_swap_exe()
        else:
            # Running as a .py - can update and re-exec in place
            update = input("Update and restart? (y/n): ").strip().lower()
            if update == 'y':
                with open(script_path, 'w', encoding='utf-8') as f:
                    f.write(remote_content)
                print("\u2713 Script updated. Restarting...\n")
                os.execv(sys.executable, [sys.executable] + sys.argv)

    except subprocess.CalledProcessError:
        pass  # No remote or not a git repo - skip silently
    except Exception:
        pass  # Never block startup due to update errors


def _hot_swap_exe():
    """Replace the running exe with the version from origin/main, then restart."""
    exe_path = sys.executable
    exe_name = os.path.basename(exe_path)
    git_path = f"origin/main:dist/{exe_name}"

    try:
        result = subprocess.run(
            ["git", "show", git_path],
            capture_output=True, check=True
        )
        new_exe_bytes = result.stdout
    except subprocess.CalledProcessError:
        print("Could not retrieve new exe from origin/main:dist/ — skipping update.")
        return

    with open(exe_path, 'rb') as f:
        current_bytes = f.read()

    if new_exe_bytes == current_bytes:
        return  # Binaries are identical despite .py differing; nothing to do

    update = input("Update and restart? (y/n): ").strip().lower()
    if update != 'y':
        return

    temp_exe = exe_path + ".new"
    bat_path = exe_path + "_updater.bat"
    pid = os.getpid()

    with open(temp_exe, 'wb') as f:
        f.write(new_exe_bytes)

    # Batch script: wait for us to exit, swap files, restart, self-delete
    bat_content = (
        "@echo off\n"
        f"powershell -command \"Wait-Process -Id {pid} -ErrorAction SilentlyContinue\"\n"
        f"move /y \"{temp_exe}\" \"{exe_path}\"\n"
        f"start \"\" \"{exe_path}\"\n"
        "(goto) 2>nul & del \"%~f0\"\n"
    )
    with open(bat_path, 'w') as f:
        f.write(bat_content)

    print("\u2713 Update downloaded. Restarting...")
    subprocess.Popen(
        ["cmd", "/c", bat_path],
        creationflags=subprocess.DETACHED_PROCESS | subprocess.CREATE_NEW_PROCESS_GROUP,
        close_fds=True
    )
    sys.exit(0)


def main():
    print("=== Unreal Engine Branch Setup Tool ===\n")

    check_for_self_update()

    # Find .uproject file
    uproject = find_uproject()
    if not uproject:
        input("Press Enter to exit")
        sys.exit(1)
    
    print(f"Found project: {uproject}\n")
    
    # Pull latest from origin/main
    print("Fetching latest changes from origin/main...")

    try:
        subprocess.run(["git", "fetch", "origin"], check=True)
        subprocess.run(["git", "checkout", "main"], check=True)
        subprocess.run(["git", "pull", "origin", "main"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"❌ Git error: {e}")
        input("Press Enter to exit")
        sys.exit(1)
    
    # Get user input
    initials = input("\nEnter your initials: ").strip()
    description = input("Enter a short description of what you're working on: ").strip()
    
    # Create branch name (remove whitespace)
    branch_name = f"{initials}-{description}"
    branch_name = re.sub(r'\s+', '', branch_name)
    
    # Create and checkout new branch
    print(f"\nCreating branch: {branch_name}")
    try:
        subprocess.run(["git", "checkout", "-b", branch_name], check=True)
        print(f"✓ Branch '{branch_name}' created successfully!\n")
    except subprocess.CalledProcessError as e:
        print(f"❌ Error creating branch: {e}")
        input("Press Enter to exit")
        sys.exit(1)
    
    # Ask if user wants to generate solution files and build
    generate = input("Generate Visual Studio solution files? (y/n): ").strip().lower()
    
    if generate == 'y':
        if generate_project_files(uproject):
            build = input("\nBuild the project now? (y/n): ").strip().lower()
            if build == 'y':
                build_project(uproject)
    
    print("\n" + "="*50)
    print("✓ Setup complete! You can now start working on your changes.")
    print("="*50)
    
    input("\nPress Enter to exit")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nOperation cancelled by user.")
        sys.exit(0)