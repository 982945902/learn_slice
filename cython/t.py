import asyncio
import sys

loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)

async def task():
    await asyncio.sleep(1)
    print("task",flush=True,file=sys.stderr)

_ = asyncio.run_coroutine_threadsafe(task(),loop)

loop.run_forever()
loop.close()