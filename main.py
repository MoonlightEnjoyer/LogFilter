from kivy.core.window import Window
from kivy.lang import Builder
from kivy.app import App
import re


class ExampleApp(App):

    path = ''

    filter = ''

    kvfile = None

    def filter_file(self, event):
        with open(self.path, 'r') as file:
            with open(f'{self.path[:-4]}_filtered.txt', 'w') as output:
                for line in file:
                    if len(re.findall(self.filter, line)) > 0:
                        output.write(line)


    def append_filter(self, event):
        if len(self.filter) == 0:
            self.filter += f'({self.kvfile.ids.filterTextInput.text})'
        else:
            self.filter += f'|({self.kvfile.ids.filterTextInput.text})'
        print('###################')
        print(self.filter)

    def on_drop_file(self, window, file_path, x, y):
        self.path = file_path.decode('utf-8')
        
        with open(self.path, 'r') as log_file:
            counter = 0
            for line in log_file:
                self.kvfile.ids['fileText'].text += f'{line}\n'
                counter += 1
                if counter >= 25:
                    break

    def build(self):

        self.kvfile = Builder.load_file('./gui.kv')


        Window.bind(on_drop_file=self.on_drop_file)

        self.kvfile.ids['add_button'].bind(on_press = self.append_filter)

        self.kvfile.ids['search_button'].bind(on_press = self.filter_file)

        
        return self.kvfile

 
if __name__ == "__main__":
    ExampleApp().run()