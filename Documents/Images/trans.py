import os
import base64

with open(os.path.join(os.getcwd(), "output1.md"), 'w+') as output:
    for dirpath, dirnames, filenames in os.walk(os.getcwd()):
        for filename in filenames:
            if os.path.splitext(filename)[1] == '.png':
                with open(os.path.join(dirpath, filename), "rb") as img_file:
                    my_string = base64.b64encode(img_file.read())
                output.writelines(str(filename + " = " + my_string.decode('utf-8')) + '\n')
