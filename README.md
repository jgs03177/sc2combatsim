StarCraft II Combat Simulator
----------------------------

### Prerequisite
+ [git](https://git-scm.com/downloads)
+ [cmake](https://cmake.org/download/)
+ Starcraft II binary (Windows from Blizzard, Linux from [this link](https://github.com/Blizzard/s2client-proto#downloads)).

### Installation

#### Windows

Starting from the main directory, clone s2client-api fork, and build the code with cmake. A document for installation can be found [here](https://github.com/Blizzard/s2client-api/blob/master/docs/building.md).

```
git clone https://github.com/jgs03177/s2client-api.git && cd s2client-api
mkdir build && cd build
cmake ..
```

Build with Visual Studio both debug mode and release mode to get the compiled libraries. We are going to move the headers and libraries later.
Go back to the main directory and clone this project (StarCraft 2 Combat Simulator), and move the headers and libraries by executing `s2client-api/synchproject.bat` and `sc2combatsim/move.bat`. This will move from
`s2client-api/project/include` and `s2client-api/project/lib` to `sc2combatsim/include` and `sc2combatsim/lib`, respectively. Then, build the simulator code with cmake.

```
cd ../..
git clone https://github.com/jgs03177/sc2combatsim.git && cd sc2combatsim
../s2client-api/synchproject.bat
move.bat
mkdir build && cd build
cmake ..
```

Finally, build with Visual Studio and execute the generated `sc2combatsim.exe`.

#### Linux
Starting from the main directory, clone s2client-api fork and build the code. A document for installation can be found [here](https://github.com/Blizzard/s2client-api/blob/master/docs/building.md).

```
git clone https://github.com/jgs03177/s2client-api.git && cd s2client-api
mkdir build && cd build
cmake ..
make
```

Go back to the main directory and clone this project (StarCraft 2 Combat Simulator), and move the headers and libraries by executing `sc2combatsim/move.sh`. Then, build the simulator code. 

```
cd ../..
git clone https://github.com/jgs03177/sc2combatsim.git && cd sc2combatsim
sh move.sh
mkdir build && cd build
cmake ..
make
```

Finally, execute the generated executable `SC2CombatSim`.

### Settings

You can modify simulation settings by modifying `config/config.json`.

```
{
    "simmode": "PvP",
    "numround": 5000,
    "numrepeat": 1,
    "stepsize": 5,
    "port": 10000,
    "mapname": "D:/Repositories/sc2combatsim/maps/Example/SimFogPlain.SC2Map",
    "player1": {
        "offset": [0.0, -20.0],
        "race": 0,
        "name": "player1"
    },
    "player2": {
        "offset": [0.0, 20.0],
        "race": 0,
        "name": "player2"
    },
    "combin1":{
        "inputpath": "",
        "limit_ore": 10000,
        "limit_gas": 5000,
        "limit_food": 150,
        "index": 2
    },
    "combin2":{
        "inputpath": "",
        "limit_ore": 10000,
        "limit_gas": 5000,
        "limit_food": 150,
        "index": 3
    }
}
```

+ `simmode`: One of `PvP`, `PvC`, `CvC`. In `CvC` mode combats are simulated with 2 agents where both agents are controlled by Blizzard's StarCraft II AIs. In `PvP` mode both agents are controlled from the code in `player.cpp` and `player.h`. In `PvC` mode, `player1 = P` and `player2 = C`.
+ `numround`: The number of simulations of new combats.
+ `numrepeat`: The number of repeats of the same combats.
+ `stepsize`: The number of intervals between the execution of the code. Greater value makes faster simulations, but less execution of the code.
+ `port`: port to use (`port`~`port+2` will be used).
+ `mapname`: Map name or path to execute the simulation.
+ `offset`: The location of the generated squad is defined by `map center coordinate` + `offset`.
+ `limit_ore`, `limit_gas`, `limit_food`: The limitation of resources forming a squad in simulation.
+ `index`: Defines which units to use in simulation. `1`: Terran ground units, `2`: zerg ground units, `3`: Protoss ground units. See `combinator.cpp`.

### Application

[Predict combat outcomes of two armies and optimizing a unit-combination of an army](https://github.com/jgs03177/pcooa-sc2)


### Papers

+ [Predicting Combat Outcomes and Optimizing Armies in StarCraft II by Deep Learning](https://authors.elsevier.com/c/1dUHX3PiGTH-G1) 

```
@article{Lee2021,
  doi = {10.1016/j.eswa.2021.115592},
  url = {https://doi.org/10.1016/j.eswa.2021.115592},
  year = {2021},
  month = dec,
  publisher = {Elsevier {BV}},
  volume = {185},
  pages = {115592},
  author = {Donghyeon Lee and Man-Je Kim and Chang Wook Ahn},
  title = {Predicting combat outcomes and optimizing armies in {StarCraft} {II} by deep learning},
  journal = {Expert Systems with Applications}
}
```

+ [BattleNet: Capturing Advantageous Battlefield in RTS Games (Student Abstract)](https://doi.org/10.1609/aaai.v34i10.7197)

```
@article{Lee2020,
  doi = {10.1609/aaai.v34i10.7197},
  url = {https://doi.org/10.1609/aaai.v34i10.7197},
  year = {2020},
  month = apr,
  publisher = {Association for the Advancement of Artificial Intelligence ({AAAI})},
  volume = {34},
  number = {10},
  pages = {13849--13850},
  author = {Donghyeon Lee and Man-Je Kim and Chang Wook Ahn},
  title = {{BattleNet}: Capturing Advantageous Battlefield in {RTS} Games (Student Abstract)},
  journal = {Proceedings of the {AAAI} Conference on Artificial Intelligence}
}
```

### Dependencies

This project contains code from:

+ [lodepng](https://lodev.org/lodepng/)

+ [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

### Useful Links

+ [s2client-proto](https://github.com/Blizzard/s2client-proto)

+ [CommandCenter](https://github.com/davechurchill/commandcenter)

+ [Getting Started with the Starcraft II Editor](https://s2editor-tutorials.readthedocs.io/en/master/index.html)

+ [StarCraft II Editor Guides](https://s2editor-guides.readthedocs.io/)

+ [SC2Mapster/Tutorials/Artificial Intelligence](https://sc2mapster.gamepedia.com/Tutorials/Artificial_Intelligence)

+ [Galaxy Editor Reference](https://mapster.talv.space/galaxy/reference)

+ [Battle.net API](https://develop.battle.net/)

### Known Issues

This simulator can run on WSL (Windows Subsystem for Linux), but the simulator crashes unexpectedly within 1 day.
