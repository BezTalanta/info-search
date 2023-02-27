import os
from sortedcontainers import SortedSet

from django.shortcuts import render, redirect
from django.views import View
from django.core.paginator import Paginator
from django.template import loader
from django.http import HttpResponse
from django.urls import reverse
from django.conf import settings

from .utils import (
    parse_all_pages,
    parse_dirty_to_pure,
    HOW_MANY_GAMES_IN_ONE_PAGE,
    Engine,
)

E = Engine()

# 2900 - the latest page
class Home(View):
    def get(self, request):
        # Be carefully
        # parse_all_pages(3000, delete=False, start_page_index=2201, threads_amount=10)
        # parse_dirty_to_pure()
        #

        if 'page' not in request.session:
            request.session['page'] = 1
            request.session.modified = True

        page_num = request.session['page']

        games, banned = E.get_games_by_page(page_num)

        return render(request, 'logic/home.html', {
            'games': games,
            'banned': banned,
            'page': page_num,
        })

    def post(self, request):
        page = request.session['page']
        if 'left_arrow' in request.POST and page > 1:
            request.session['page'] = page - 1

        if 'right_arrow' in request.POST and page < 72501:
            request.session['page'] = page + 1

        if 'exact_page' in request.POST:
            request.session['page'] = int(request.POST['exact_page'])

        request.session.modified = True
        return redirect(reverse('logic:home'))


def show_static_html(request):
    index = request.GET.get('num', False)
    if index:
        html = loader.get_template(settings.BASE_DIR / ("dirty_dump/" + str(index) + ".html"))
        return HttpResponse(html.render())
    return redirect(reverse('logic:home'))
