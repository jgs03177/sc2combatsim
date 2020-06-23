
# StarCraft 2 Combat Simulator

### Installation

#### Windows
Get Starcraft binary from the blizzard homepage.
Get [git](https://git-scm.com/downloads).
Get [cmake](https://cmake.org/download/).

Get s2client-api fork. The document can be found [here](https://github.com/Blizzard/s2client-api/blob/master/docs/building.md).

```
git clone https://github.com/Blizzard/s2client-api.git && cd s2client-api
mkdir build && cd build
cmake ..
```

Build with Visual Studio to get the libraries, and move the libraries.

```
move
```

Get this repository.

```
git clone sc2combatsim && cd sc2combatsim
mkdir build && cd build
cmake ..
```

Finally, build with Visual Studio.

#### Linux
Get [StarCraft Linux binary](https://github.com/Blizzard/s2client-proto#downloads).
Get [git](https://git-scm.com/downloads).
Get [cmake](https://cmake.org/download/).

Get s2client-api fork. The document can be found [here](https://github.com/Blizzard/s2client-api/blob/master/docs/building.md).

```
git clone https://github.com/Blizzard/s2client-api.git && cd s2client-api
mkdir build && cd build
cmake ..
make
sh move.sh
```

make these libraries and move.

Get this repository.

```
git clone sc2combatsim && cd sc2combatsim
mkdir build && cd build
cmake ..
make
```

make and execute.

### Dependencies

[s2client-api](https://github.com/Blizzard/s2client-api)

[lodepng](https://lodev.org/lodepng/)

[jsoncpp](https://github.com/open-source-parsers/jsoncpp)

### Useful Links

[s2client-proto](https://github.com/Blizzard/s2client-proto)

[CommandCenter](https://github.com/davechurchill/commandcenter)

[Getting Started with the Starcraft II Editor](https://s2editor-tutorials.readthedocs.io/en/master/index.html)

[StarCraft II Editor Guides](https://s2editor-guides.readthedocs.io/)

[SC2Mapster/Tutorials/Artificial Intelligence](https://sc2mapster.gamepedia.com/Tutorials/Artificial_Intelligence)

[Galaxy Editor Reference](https://mapster.talv.space/galaxy/reference)

[Battle.net API](https://develop.battle.net/)

### known issues:
WSL (Windows Subsystem for Linux) can run this simulator, but it stops running in about 1 day.
