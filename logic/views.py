import requests
import os
from pathlib import Path
from bs4 import BeautifulSoup

from django.shortcuts import render
from django.views import View

from .utils import (
    get_url,
    parse_all_pages,
)


class Home(View):
    def get(self, request):
        parse_all_pages(2000, delete=False, start_page_index=41, threads_amount=20)

        return render(request, 'logic/home.html', {
        })

    def post(self, request):
        return render(request, 'logic/home.html', {

        })
