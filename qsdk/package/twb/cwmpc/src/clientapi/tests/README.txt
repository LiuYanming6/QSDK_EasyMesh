

runtest is a test program that reads lines from stdin. 
Each line contains the name of an API function and
 	it's arguments. In the form:
 	           <api-name> [argument [[argument] ...] ] ]
 	Such as:
 	           SetParameterValue InternetGatewayDevice.ManagementServer.Username xyz
 
 	The pseudo functions are
 	           delay seconds
 	           
The *.tst files contains directives to the runtest program.


 For example:
    $ ./runtest localhost <getrpc.tst
    