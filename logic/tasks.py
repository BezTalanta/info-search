import time, random, requests
from bs4 import BeautifulSoup as bs
from celery import shared_task
import config.cute_logging as cl


@shared_task()
def test(number):
    ri = random.randint(2, 6)
    print(f'{ri} test got')
    time.sleep(ri)
    cl.message(f'Task with number: {number}, got {ri} seconds', cl.GOOD)
