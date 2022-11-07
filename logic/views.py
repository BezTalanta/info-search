import requests
import os
from pathlib import Path
from bs4 import BeautifulSoup

from django.shortcuts import render
from django.views import View

from .utils import (
    get_url,
    get_totally_amount_of_pages,
)

# How many pages you want, you can change it. 0 will parse all pages
# Note: on one page - 25 games
WISH_PAGE_AMOUNT = 1

class Home(View):
    def get(self, request):
        # Create folder 'original_dump' if it's not exist
        if not os.path.isdir(Path(__file__).parent.parent.resolve() / 'dirty_dump/'):
            os.makedirs(Path(__file__).parent.parent.resolve() / 'dirty_dump/')
        # Delete all files in dirty dump
        else:
            for file in os.listdir('dirty_dump/'):
                os.remove(os.path.join('dirty_dump/', file))

        # Save games to dirty dump
        wish_page_amount = get_totally_amount_of_pages(WISH_PAGE_AMOUNT)

        # return render(request, 'logic/home.html', {
        # })

        # This variables doesn't be changed manually
        totally_games_amount = 1
        totally_pages_amount = 1
        status = True
        while status:
            current_page = requests.get(get_url(totally_pages_amount)).text
            bs = BeautifulSoup(current_page, 'html.parser')
            la = bs.find_all('a')
            for a in la:
                if a.get('class') is not None and \
                    a.get('class')[0] == 'search_result_row':
                    with open(f'dirty_dump/{totally_games_amount}.txt', 'w') as f:
                        f.write(requests.get(a.get('href')).text)
                    totally_games_amount += 1
            totally_pages_amount += 1
            # Uploading result output
            print(f'Have already parsed {totally_games_amount - 1} games and {totally_pages_amount - 1} pages...')
            # For debugging, if we want some pages
            if wish_page_amount > 0 and \
                totally_pages_amount > wish_page_amount:
                status = False
                break
            status = True if len(la) > 0 else False
        print('All games have been parsed!')

        return render(request, 'logic/home.html', {
        })

    def post(self, request):
        return render(request, 'logic/home.html', {

        })
