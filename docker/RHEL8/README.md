docker build -f docker/RHEL8/Dockerfile -t rsender:rhel8 .

cid=$(docker create rsender:rhel8)

docker cp "$cid:/app/bin/rsender" ./package/rhel8/