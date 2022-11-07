from html.parser import HTMLParser
import requests
from bs4 import BeautifulSoup


class Parser(HTMLParser):
    def handle_starttag(self, tag, attrs):
        if tag == 'a':
            print(attrs)

    def handle_endtag(self, tag):
        pass

    def handle_data(self, data):
        pass


def get_url(page_index):
    return f'https://store.steampowered.com/search/?sort_by=&sort_order=0&page={page_index}&hide_filtered_results_warning=1&ignore_preferences=1'


def get_totally_amount_of_pages(wish):
    if wish != 0:
        return wish
    bs = BeautifulSoup(requests.get(get_url(1)).text, 'html.parser').find_all('div', {'class': 'search_pagination_right'}).find('a')
    print(bs)
    # for b in bs:
    #     print(b[])
    return 0
