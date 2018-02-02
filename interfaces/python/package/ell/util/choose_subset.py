from .. import platform

def choose_subset(labels, path, labels_per_column=100):
    import ipywidgets as widgets
    saved_labels=[]
    import os.path
    if os.path.isfile(path):
        with open(path) as label_file:
            saved_labels = [line.rstrip('\n') for line in label_file.readlines()]
    label_set = set(saved_labels)
    layout = widgets.Layout(max_width='120px', width='120px', height='16px')
    
    def make_checkbox(desc):
        c = widgets.Checkbox(description=desc, layout=layout, value=desc in label_set)
        def update(ch):
            nonlocal label_set
            if ch.new:
                label_set |= {ch.owner.description}
            else:
                label_set -= {ch.owner.description}
            with open(path, 'wt') as label_file:
                label_file.write('\n'.join(label_set))
        c.observe(update, names='value')
        return c
    
    boxes = [make_checkbox(lab) for lab in labels]
    display(widgets.HBox([widgets.VBox(boxes[i:i+labels_per_column]) for i in range(0, len(labels), labels_per_column)]))
    return label_set

if platform.has_magic:
    # improve the layout of checkboxes on the notebook page
    from IPython.core.display import HTML, display
    display(HTML("<style>.widget-inline-hbox .widget-label {width:0} .widget-checkbox input[type='checkbox'] {margin: 0} .widget-box {overflow: visible}</style>"))
