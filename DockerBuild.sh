

docker build -t "build:latest" .

#SOURCELIBRARY_BASE=/SourceLibrary ./Build.sh

docker run -e SOURCELIBRARY_BASE=/SourceLibrary -v /home/steve/BlockWorks/Reactor:/BuildArea -v ~/BlockWorks/ProjectLayout/SourceLibrary:/SourceLibrary -i  -t "build:latest"


