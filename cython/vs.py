from fastapi import FastAPI
import uvicorn
import asyncio

import asyncio
import uvloop
asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

# app = FastAPI()


# @app.get("/test")
# async def test():
#     # await asyncio.sleep(0.01)
#     return {"file_path": "test"}

from blacksheep import Application, get
app = Application()

@get("/test")
async def test():
    #await asyncio.sleep(0.01)
    return f"test"

if __name__ == "__main__":

    uvicorn.run(app=app, host="0.0.0.0", port=8888,
                reload=False, log_level="critical")
