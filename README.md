
# StarCraft 2 Combat Simulator

```
# Install SC2 API headers
$ sudo mkdir -p /opt/local/include
$ sudo cp -R include/{sc2api,sc2renderer,sc2utils} /opt/local/include
$ sudo cp -R build/generated/s2clientprotocol /opt/local/include

# Install protobuf headers
$ sudo cp -R contrib/protobuf/src/google /opt/local/include/sc2api

# Install SC2 API libraries
$ sudo mkdir -p /opt/local/lib/sc2api
$ sudo cp build/bin/{libcivetweb,libprotobuf,libsc2api,libsc2lib,libsc2protocol,libsc2utils}.a /opt/local/lib/sc2api
```

## Dependencies

[s2client-api](https://github.com/Blizzard/s2client-api)

[lodepng](https://lodev.org/lodepng/)

[jsoncpp](https://github.com/open-source-parsers/jsoncpp)

## Links

[s2client-proto](https://github.com/Blizzard/s2client-proto)

[CommandCenter](https://github.com/davechurchill/commandcenter)

[Getting Started with the Starcraft II Editor](https://s2editor-tutorials.readthedocs.io/en/master/index.html)

[StarCraft II Editor Guides](https://s2editor-guides.readthedocs.io/)

[SC2Mapster/Tutorials/Artificial Intelligence](https://sc2mapster.gamepedia.com/Tutorials/Artificial_Intelligence)

[Galaxy Editor Reference](https://mapster.talv.space/galaxy/reference)

[Battle.net API](https://develop.battle.net/)
