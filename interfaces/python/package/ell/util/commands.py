from IPython.core.display import display, HTML


def print(text):
    display(HTML('<pre style="font-size:80%; line-height:110%">' + text + '</pre>'))

def run(command, echo=False):
    try:
        if echo:
            print('<b>' + command + '</b>')

        proc = Popen(command, stdout=PIPE, stderr=PIPE, bufsize=1)
        output = '';
        for line in iter(proc.stdout.readline, b''):
            output += line.decode('utf-8')
        for line in iter(proc.stderr.readline, b''):
            output += line.decode('utf-8')
        print(output)    
    except FileNotFoundError:
        print("cannot run command: " + command)

def run_swig():
    pass

def run_llc():
    pass
