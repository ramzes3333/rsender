docker build -f docker/RHEL8/Dockerfile -t rsender:rhel8 .
docker cp "$(docker create rsender:rhel8):/app/bin/rsender" ./package/rhel8/