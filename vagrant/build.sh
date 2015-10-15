#!/bin/sh

vagrant up



vagrant ssh -c "export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64; rm -rf shm && cp -r /repos/ shm && \
								cd shm && \
								mvn -f shmfmi-server/pom.xml package && \
								./ccompile.sh && \
								./x-compile.sh"
# && \
#								cp -r builds/output /build-output"

# small fix for libvirt

VMIP=`vagrant ssh -c "ip address show eth0 | grep 'inet ' | sed -e 's/^.*inet //' -e 's/\/.*$//'"`

VMIP=`echo $VMIP | grep -Eo '([0-9]{1,3}\.){3}[0-9]{1,3}' | sort | uniq`
echo VM ip is $VMIP

echo add public key
KEY=`cat ssh/id_rsa.pub`

TT=output
mkdir -p $TT

vagrant ssh -c "echo $KEY >> ~/.ssh/authorized_keys"

chmod 600 ssh/id_rsa

rsync -e "ssh -o StrictHostKeyChecking=no -i ssh/id_rsa" -rv --prune-empty-dirs --include '*/' --include '*.so' --include '*.dll' --include 'redirector-*.err' --include 'redirector-*.out' --exclude '*' vagrant@$VMIP:/home/vagrant/shm/builds/output/ $TT

rsync -e "ssh -o StrictHostKeyChecking=no -i ssh/id_rsa" -rv --prune-empty-dirs --include '*/' --include '*.so' --include '*.dll' --include '*.java' --include 'redirector-*.err' --include 'redirector-*.out' --exclude '*' vagrant@$VMIP:/home/vagrant/shm/shmfmi-server/target $TT

vagrant halt
