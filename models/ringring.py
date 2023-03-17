# A REST-API webhook endpoint for OctoEverywhere to make a noise when a print is done.
import os

import fastapi
import uvicorn

import logging
from rich.logging import RichHandler

FORMAT = "%(message)s"
logging.basicConfig(
    level="NOTSET", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
)

log = logging.getLogger("rich")

# Create HTTP Server
app = fastapi.FastAPI()


def say(msg: str):
    os.system(f"say '{msg}'")


@app.post('/webhook')
async def webhook(request: fastapi.Request):
    # Get the JSON body
    obj = await request.json()
    prog = obj.get('Progress')
    if prog == 100:
        # Make a noise
        say('Meow Meow Meow! Meow Meow Meow Meow Meow!')

    elif obj.get("EventType") == 6:
        say(f"{obj.get('PrinterName')} Progress: {prog}%")

    log.info(obj)

    return 'OK'


if __name__ == '__main__':
    # Run the server
    uvicorn.run(app, host='0.0.0.0', port=8484)