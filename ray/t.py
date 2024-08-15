import ray

@ray.remote
def gen():
    return "a-gen"

a = gen.remote()

@ray.remote
def cus(a):
    print("debug",a)

b = cus.remote([a])

print("end",ray.get(b))