# move.sh
# repo & build directory.

repo_directory="../cpp-sc2"
build_directory="../cpp-sc2/build"

echo "cpp-sc2's directory is set to ${repo_directory}"
echo "cpp-sc2's build directory is set to ${build_directory}"

# install sc2api headers
sudo mkdir -p /opt/local/include
sudo cp -R ${repo_directory}/include/sc2api /opt/local/include
sudo cp -R ${repo_directory}/include/sc2lib /opt/local/include
sudo cp -R ${repo_directory}/include/sc2renderer /opt/local/include
sudo cp -R ${repo_directory}/include/sc2utils /opt/local/include
sudo cp -R ${build_directory}/generated/s2clientprotocol /opt/local/include

# install protobuf headers
sudo cp -R ${repo_directory}/contrib/protobuf/src/google /opt/local/include/sc2api

# install sc2api libraries
sudo mkdir -p /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libcivetweb.a /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libprotobuf.a /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libsc2api.a /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libsc2lib.a /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libsc2protocol.a /opt/local/lib/sc2api
sudo cp ${build_directory}/bin/libsc2utils.a /opt/local/lib/sc2api

echo "done!"
