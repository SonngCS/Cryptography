import os
from cryptography.fernet import Fernet

files = [f for f in os.listdir() if(f != "encrypt.py" and f != "decrypt.py" and f != "Secrets.key" and os.path.isfile(f))]

with open("Secrets.key", "rb") as key:
    secret = key.read()
key.close()

for f in files:
    with open(f, "rb") as file:
        content = file.read()
    file.close()
    contentModified = Fernet(secret).decrypt(content)
    
    with open(f, "wb") as file:
        file.write(contentModified)
    file.close()