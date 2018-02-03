# cryptone_web <br>
Cryptoprotocol Beta 1.0 <br>
Add ping command to server - done  <br>
Client/Server data exchange - done <br>
Add subclients registration for username - done <br>
Users registration - done <br>
RSA key exchange on fly - done <br>
AES-256 strong encryption channel <br>
Inflate/Deflate - compression of trans mitted data - done <br>
Shorthand masking of transmitted data - done <br>
Encapsulation of data in any data transfer protocol - done <br>
<hr>
<a href="https://secureline.online/">Project server: secureline.online</a><br>
<hr>
 
AddNewClient.h function:<br> 
```cpp
/*
function:
(use only for add new client to exists user)
Temp user Client/Server final HandShake function exchange of crypto keys and public keys

input vars:
unsigned char* UserID - UseridHash
unsigned char* AESKey - AES key
unsigned char* AESVector - AES Vector
unsigned char* KeyPub - Public key
char* Servername - Server name
unsigned char* strPwd - Container password

return:
NULL - FAIL
1 - OK
*/
int AddNewClientRegHandshake(unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, unsigned char*KeyPub, char* Servername, unsigned char* strPwd);

/*
Function:
Adding a new client to an existing user.
Remeber, username is Global var!

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
int AddNewClientToUser(char* Servername);
```
<br>
AskPasswords.h<br>
```cpp
/*
Function:
Ask master password for add new subclients for Username
The maximum size of the master password is 32 characters.
return:
NULL - FAILED
char* - Password String
*/
char* AskMasterPassword();

/*
Function:
Asks the password for the crypto container, it stores the settings and important variables
The maximum size of the crypto container password is 32 characters.
return:
NULL - FAILED
char* - Password String
*/
char* AskContainerPassword();
```
