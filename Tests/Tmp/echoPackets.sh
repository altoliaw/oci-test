sudo tcpreplay -i ens224 $(pwd)/ExternalData/172.22.2.51_1521.v1.pcap
if [ $? -ne 0 ] 
then
	echo "$(pwd)/ExternalData/172.22.2.51_1521.v1.pcap"
	echo "The pwd shall be located in the project root directory."
fi