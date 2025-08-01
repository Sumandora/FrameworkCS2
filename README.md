# This branch is still unfinished, you can play with it but there are several minor and major bugs.

# FrameworkCS2
A fully-featured training software for Counter Strike: 2, made for Linux.

> [!IMPORTANT]  
> This project is very much a WIP, don't expect too much from it right now.

### WARNING: This program is being detected by Valve. Usage on official, custom, or private servers will most likely result in punishment from Valve!

## Installation
### Dependencies
<details>
  <summary> Debian and derivatives </summary>
  
  ```sh
  apt-get install gdb git cmake make build-essential libvulkan-dev libx11-dev libxext-dev gcc-14 g++-14
  ```

  Some older distributions, like Ubuntu and Debian may need to specify GCC14 explicitly, by setting the `CC=gcc-14` and `CXX=g++-14` environment variables when invoking the build script.

  Tested derivatives:
  - Ubuntu (LTS and Latest)
  - Debian Unstable and Testing
  - Devuan
</details>

<details>
  <summary> Arch Linux and derivatives </summary>

  ```sh
  pacman -S gdb base-devel cmake git vulkan-icd-loader vulkan-headers libx11 libxext
  ```

  Tested derivatives:
  - Arch Linux
  - Artix Linux
  - CachyOS
</details>

<details>
  <summary> Fedora </summary>

  ```sh
  dnf install gdb git cmake make gcc-c++ vulkan-loader-devel vulkan-headers libXext-devel
  ```
</details>

<details>
  <summary> OpenSUSE </summary>

  ```sh
  zypper install -y git cmake make gcc-c++ binutils-devel vulkan-devel libXext-devel freetype2-devel ccache
  ```

  Only Tumbleweed is being tested, Leap installs a very outdated GCC version by default.
</details>
<details>
  <summary> Gentoo </summary>

  ```sh
  emerge dev-vcs/git dev-build/cmake dev-util/vulkan-headers dev-util/vulkan-tools
  ```
</details>
These might be outdated, in that case please open an issue or a pull request, which updates the package names.

### Download
```sh
git clone https://github.com/Sumandora/FrameworkCS2.git
```

The build script can be used to build the program.
```sh
./Build.sh
```

If you want to supply additional compiler arguments use `CMAKE_CXX_FLAGS`.
```sh
./Build.sh -D CMAKE_CXX_FLAGS="-fstack-protector-strong"
```
Some of these might break the resulting binary however.

An ELF-Binary will be built in the "Build"-subdirectory, which will be created by the script.

### Custom library names

In case you want to build the project using a different name you can do something like this:
```sh
echo "MyCoolProjectName" > ProjectName
git clean -fdx # Remove all uncommitted files, like the build outputs
# Rebuild here using Debug.sh or Build.sh
```

## Usage
A simple inject script using a debugger like GDB is provided.  
Make sure to run it as root user.
```sh
./Load.sh
```

After the script loaded the program into the game it will set the ptrace_scope to 3.  
This means that you will not be able to use ptrace after loading.  
To use ptrace again you will need to reboot.  
Please note that this project is not the only software using ptrace, it may be used widely on your system.

Make sure to use the script in a state, where the game is not changing frequently.  
- Don't inject before you can see the main menu
- Don't inject when you are loading a level

### Grenade Helper
By default, no grenades are available.  
The grenade format can be found in the `Source/Serialization/GrenadeSerialization.hpp` file.  
A friend of mine, [snek](https://github.com/jovvik/), generously provided his collection of grenades [here](https://l2p.sn3k.dev/grenades.json).  
Download this file to `~/.config/FrameworkCS2/grenades.json`.  

```sh
mkdir -p ~/.config/FrameworkCS2
wget 'https://l2p.sn3k.dev/grenades.json' -O ~/.config/FrameworkCS2/grenades.json
```

### Chams
2 Materials (and their versions without Z-Buffering) are provided by default, you can add more by adding them to the `~/.config/FrameworkCS2/Materials`.
For references you can take a look at the default ones, you can also delete them. The directory is regenerated when it doesn't exist.

### Player model changer
The model changer will display all default models by default. Custom models are supported however, but must be installed first.
Model files go in `~/.steam/steam/steamapps/common/Counter-Strike Global Offensive/game/csgo/characters/models` directory.
You can find models on several places like [GameBanana](https://gamebanana.com/mods/cats/25415).
When downloading models you generally want to end up with a file structure like this:

```
/path/to/cs2/game/csgo/characters/models/[model author]/[model name]/[model name].vmdl
```

Some model authors put more subdirectories in, however as long as the model (the `.vmdl` file) is part of the `characters/models/` directory, you should have no problems.

Please make sure you don't have duplicate `characters/models/`, because some model authors duplicate these directories inside their releases to make clear where to put the files.
Also make sure you are in the `game` subdirectory, as CS:GO and CS:2 have a very similar file structure.

CS:GO models are not compatible with CS:2, however they can be ported.
There are several guides explaining how to port models, both in video-form and text-form.
I have seen people sell ported models for real-life money, which I actively vouch against buying, because porting isn't too difficult and the original authors of the models don't get any money or credit from it.

When adding new models the menu should automatically pick up on them and no restart/re-inject is required, although it may take a few seconds, because the menu only searches for new models every 5 seconds.

### Contribution
Contributions are welcome  
Issues and Pull Requests can help improve the program

#### Setting up a workspace (Example, your setup can differ)
- Fork the repository
```sh
git clone [URL] # Replace URL with your fork of the repository
./Build.sh # Check for initial build errors
** Make changes **
./Debug.sh # The debug script will build and load a debug build, which can also be analyzed using a debugger of your choice (run this as root)

git add file1 file2 # If you added new files, you have to mark them to be tracked, if you didn't add any files, you can skip this step.
git commit -a # Make a commit with all changed files. You may have to set the 'EDITOR' variable, because you have to write a commit message. Please write a small and compact message explaining what you have done.
git push
```
You should have pushed your changes to your fork.  
If you want to propose the changes in the mainline release, you can open a pull request on GitHub.
