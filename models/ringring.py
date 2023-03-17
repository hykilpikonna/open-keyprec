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


# Create a webhook endpoint
@app.post('/webhook')
async def webhook(request: fastapi.Request):
    # log.info(f"Request: {request}")
    # log.info(f"Request Headers: {request.headers}")
    # log.info(f"Request Query: {request.query_params}")

    # Get the JSON body
    obj = await request.json()
    if obj.get('Progress') == 100:
        # Make a noise
        os.system("say 'Meow Meow Meow! Print Complete, Print Complete, Meow Meow Meow Meow Meow!'")

    log.info(obj)

    return 'OK'


if __name__ == '__main__':
    # Run the server
    uvicorn.run(app, host='0.0.0.0', port=8484)