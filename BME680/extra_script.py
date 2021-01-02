Import("env")

# # please keep $SOURCE variable, it will be replaced with a path to firmware

# Python callback
def on_upload(source, target, env):
    firmware_path = str(source[0])
    # do something
    # env.Execute("C:\\Users\\cmeinert\\AppData\\Local\\Programs\\Python\\Python39\\python -V")
    # env.Execute("python -V")
    env.Execute("..\\ota_updater.py $UPLOADERFLAGS " + firmware_path)

env.Replace(UPLOADCMD=on_upload)