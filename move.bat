@mkdir lib\
@mkdir include\

@set "api_repo_directory=..\s2client-api"
@set "sim_repo_directory=."
@echo API repository's directory is set to %api_repo_directory%
@echo Simulator repository's directory is set to %sim_repo_directory%

xcopy %api_repo_directory%\project\include\* %sim_repo_directory%\include /s /e
xcopy %api_repo_directory%\project\lib\* %sim_repo_directory%\lib /s /e

@echo done!
@pause