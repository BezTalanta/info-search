import os
import config.cute_logging as clog
from celery import shared_task
from pathlib import Path
import requests
from bs4 import BeautifulSoup as bs

HOW_MANY_GAMES_IN_ONE_PAGE = 25


def get_url(page_index):
    return f'https://store.steampowered.com/search/?sort_by=&sort_order=0&page={page_index}&hide_filtered_results_warning=1&ignore_preferences=1'


@shared_task()
def parse_some_pages(idx_first, idx_second):
    '''Parse steam pages in range between first and second indexes'''
    clog.message(f'[{idx_first}, {idx_second}: starting...] I have started parse', clog.GOOD)
    _range = idx_second + 1 - idx_first
    output_parts = {
        idx_first + int(_range * 0.5): '50%',
        idx_first + int(_range * 0.75): '75%',
        idx_first + int(_range * 0.9): '90%',
    }

    for i in range(idx_first, idx_second + 1):
        if i in output_parts:
            clog.message(f'[{idx_first}, {idx_second}: executing...] I am done by {output_parts[i]}', clog.GOOD)

        c_bs = bs(requests.get(get_url(i)).text, 'html.parser')
        games_amount = 1
        for a in c_bs.find_all('a'):
            if a.get('class') is not None and \
                a.get('class')[0] == 'search_result_row':
                    with open(f'dirty_dump/{HOW_MANY_GAMES_IN_ONE_PAGE * (i - 1) + games_amount}.html', 'w') as f:
                        f.write(requests.get(a.get('href')).text)
                    games_amount += 1
    clog.message(f"[{idx_first}, {idx_second}: Done] I finished it, my master", clog.GOOD)


def parse_all_pages(end_page_index, *args, **kwargs):
    '''
        kwargs:
            delete=False        - if True python will delete dirty_dump/ and
                                    create another one
            start_page_index=1  - defines index of start page to parse
            threads_amount=10   - defines how many threads celery will use
    '''
    if kwargs.get('delete', False) != False and kwargs['delete'] == True:
        # Create folder 'original_dump' if it's not exist
        if not os.path.isdir(Path(__file__).parent.parent.resolve() / 'dirty_dump/'):
            os.makedirs(Path(__file__).parent.parent.resolve() / 'dirty_dump/')
        # Delete all files in dirty dump
        else:
            for file in os.listdir('dirty_dump/'):
                os.remove(os.path.join('dirty_dump/', file))

    start_page_index = 1 \
        if kwargs.get('start_page_index', False) is False \
        else kwargs['start_page_index']
    _range = end_page_index + 1 - start_page_index

    threads_amount = 10 \
        if kwargs.get('threads_amount', False) is False \
        else kwargs['threads_amount']

    if threads_amount > _range or _range < 1:
        raise ValueError(
            'Your threads amount more than range or the range very low'
            )

    '''
        ppt is equal to Pages Per Threads
        Defines how many pages will be parsed by one thread
        ppt_left defines how many pages will be left
        Left pages will go to the last thread
    '''
    ppt, ppt_left  = int(_range / threads_amount), _range % threads_amount
    clog.message(f'[Starting] range: {_range}, start: {start_page_index}, end: {end_page_index}, ppt: {ppt}, left: {ppt_left}, threads: {threads_amount}', clog.ERROR)
    for i in range(threads_amount):
        sp = start_page_index + ppt * i
        # print(sp)
        # print('I will parse between: ', sp,
        #       sp + ppt - 1 + (ppt_left if i == threads_amount - 1 else 0))
        parse_some_pages.delay(sp, sp + ppt - 1 +
                               (ppt_left if i == threads_amount - 1 else 0))


@shared_task()
def parse_dirty_to_pure_sp(start, end):
    for i in range(start, end + 1):
        current_file_path = f'dirty_dump/{i}.html'
        if not os.path.isfile(current_file_path):
            clog.message(f'{current_file_path} does not exist, passing', clog.ERROR)
            continue
        with open(current_file_path, 'r') as f:
            # Get splitted text by \n
            b = bs(f.read(), 'html.parser').text.split('\n')
            # Make it more pretify
            result_text = '\n'.join([line.strip() for line in b if len(line.strip()) > 0])
            with open(f'pure_dump/{i}.txt', 'w') as f2: # 533
                f2.write(result_text)


# 53??? - 55000 missing
def parse_dirty_to_pure():
    for i in range(10): # 7500
        l = 7500 * i + 1
        parse_dirty_to_pure_sp.delay(l, l + 7500)
