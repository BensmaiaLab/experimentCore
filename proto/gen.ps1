$outDir='pyClient'
$projectDir = 'E:\repos\experimentCore\'
$PROTO_FILES = "${projectDir}proto\helloWorld.proto"

set-location $projectDir

python3 -m grpc.tools.protoc `
--python_out="$outDir" `
--grpc_python_out="$outDir" `
--proto_path="$projectDir\proto\" `
$PROTO_FILES
