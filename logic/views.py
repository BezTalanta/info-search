from django.shortcuts import render
from django.views import View

from .utils import (
    parse_all_pages,
    parse_dirty_to_pure,
)


class Home(View):
    def get(self, request):
        # parse_all_pages(3000, delete=False, start_page_index=2201, threads_amount=10)

        parse_dirty_to_pure()

        return render(request, 'logic/home.html', {
        })

    def post(self, request):
        return render(request, 'logic/home.html', {

        })
