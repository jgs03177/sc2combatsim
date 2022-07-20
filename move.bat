@mkdir lib\
@mkdir include\
@mkdir include\google\

@set "api_repo_directory=..\cpp-sc2"
@set "api_build_directory=..\cpp-sc2\build"
@set "sim_repo_directory=."
@echo cpp-sc2's directory is set to %api_repo_directory%
@echo cpp-sc2's build directory is set to %api_build_directory%
@echo sc2combatsim's directory is set to %sim_repo_directory%

xcopy %api_repo_directory%\include\* %sim_repo_directory%\include /s /e
xcopy %api_build_directory%\generated\* %sim_repo_directory%\include /s /e
xcopy %api_repo_directory%\contrib\protobuf\src\google\* %sim_repo_directory%\include\google\ /s /e

xcopy %api_build_directory%\bin\civetweb.lib %sim_repo_directory%\lib\civetweb.lib /s /e

xcopy %api_build_directory%\bin\libprotobuf.lib %sim_repo_directory%\lib\protobuf.lib /s /e
xcopy %api_build_directory%\bin\sc2api.lib %sim_repo_directory%\lib\sc2api.lib /s /e
xcopy %api_build_directory%\bin\sc2protocol.lib %sim_repo_directory%\lib\sc2protocol.lib /s /e
xcopy %api_build_directory%\bin\sc2renderer.lib %sim_repo_directory%\lib\sc2renderer.lib /s /e
xcopy %api_build_directory%\bin\sc2utils.lib %sim_repo_directory%\lib\sc2utils.lib /s /e
xcopy %api_build_directory%\bin\SDL2.lib %sim_repo_directory%\lib\SDL2.lib /s /e

xcopy %api_build_directory%\bin\libprotobufd.lib %sim_repo_directory%\lib\protobufd.lib /s /e
xcopy %api_build_directory%\bin\sc2apid.lib %sim_repo_directory%\lib\sc2apid.lib /s /e
xcopy %api_build_directory%\bin\sc2protocold.lib %sim_repo_directory%\lib\sc2protocold.lib /s /e
xcopy %api_build_directory%\bin\sc2rendererd.lib %sim_repo_directory%\lib\sc2rendererd.lib /s /e
xcopy %api_build_directory%\bin\sc2utilsd.lib %sim_repo_directory%\lib\sc2utilsd.lib /s /e
xcopy %api_build_directory%\bin\SDL2d.lib %sim_repo_directory%\lib\SDL2d.lib /s /e

@echo done!
@pause
