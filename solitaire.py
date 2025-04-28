import tkinter as tk
from tkinter import messagebox
import random
from PIL import Image, ImageTk, ImageDraw, ImageFont

class SolitaireGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Solitaire")
        self.root.resizable(False, False)

        # Dimensions du jeu
        self.WIDTH = 1200
        self.HEIGHT = 800
        self.CARD_WIDTH = 80
        self.CARD_HEIGHT = 120

        self.control_frame = tk.Frame(root)
        self.control_frame.pack(pady=10)

        self.restart_button = tk.Button(self.control_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack(side=tk.LEFT, padx=20)

        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="#2F4F4F")
        self.canvas.pack()

        # Chargement des sprites
        self.load_card_sprites()
        self.setup_game()
        self.setup_bindings()
        self.game_loop()  # Start the game loop
        
    def setup_game(self):
        # Initialisation des variables de jeu
        self.selected = None
        
        # Création du jeu de cartes
        deck = []
        for suit in ['heart', 'diamond', 'club', 'spade']:
            for value in range(1, 14):  # 1=As, 11=Valet, 12=Dame, 13=Roi
                deck.append((suit, value))
        
        # Mélange du jeu
        random.shuffle(deck)
        
        # Initialisation des piles
        self.tableau = [[] for _ in range(7)]
        self.foundation = [[] for _ in range(4)]
        
        # Distribution des cartes sur le tableau
        card_index = 0
        for i in range(7):
            for j in range(i + 1):
                self.tableau[i].append(deck[card_index])
                card_index += 1
        
        # Le reste des cartes va dans le stock
        self.stock = deck[card_index:]
        self.waste = []  # Pile de défausse (initialement vide)

    def load_card_sprites(self):
        # Génération dynamique des sprites de cartes
        self.card_back = self.create_card_sprite(back=True)
        self.cards = {}
        
        colors = {'heart': '#FF0000', 'diamond': '#FF0000', 
                 'club': '#000000', 'spade': '#000000'}
        
        for suit in ['heart', 'diamond', 'club', 'spade']:
            for value in range(1, 14):
                self.cards[(suit, value)] = self.create_card_sprite(suit, value, colors[suit])

    def create_card_sprite(self, suit=None, value=None, color='#FFFFFF', back=False):
        img = Image.new("RGBA", (self.CARD_WIDTH, self.CARD_HEIGHT), (255,255,255,255))
        draw = ImageDraw.Draw(img)
        
        # Contour de la carte avec un meilleur effet visuel
        draw.rounded_rectangle([2, 2, self.CARD_WIDTH-2, self.CARD_HEIGHT-2], 
                             radius=10, fill="#FFFFFF", outline="#000000", width=2)
        
        if back:
            # Dessin du dos de carte plus élaboré
            draw.rectangle([10, 10, self.CARD_WIDTH-10, self.CARD_HEIGHT-10], 
                         fill="#000080", outline="#000000")
            # Motif décoratif - correction des coordonnées
            for i in range(5):
                offset = 20 + i*8
                # Vérifier que les coordonnées sont valides
                if (self.CARD_WIDTH-20-i*8) > (20+i*8) and (self.CARD_HEIGHT-20-i*8) > (20+i*8):
                    draw.rounded_rectangle(
                        [offset, offset, self.CARD_WIDTH-offset, self.CARD_HEIGHT-offset],
                        radius=5, outline="#FFD700", width=1
                    )
            return ImageTk.PhotoImage(img)
        
        # Symboles et valeurs
        symbols = {'heart': '♥', 'diamond': '♦', 'club': '♣', 'spade': '♠'}
        value_map = {1: 'A', 11: 'J', 12: 'Q', 13: 'K'}
        value_str = value_map.get(value, str(value))
        
        try:
            font = ImageFont.truetype('arial.ttf', 22)
            small_font = ImageFont.truetype('arial.ttf', 18)
        except:
            font = ImageFont.load_default()
            small_font = ImageFont.load_default()
        
        # Valeur en haut à gauche
        draw.text((8, 8), value_str, fill=color, font=font)
        draw.text((8, 30), symbols[suit], fill=color, font=font)
        
        # Valeur en bas à droite (inversée)
        draw.text((self.CARD_WIDTH-22, self.CARD_HEIGHT-30), value_str, fill=color, font=font)
        draw.text((self.CARD_WIDTH-22, self.CARD_HEIGHT-52), symbols[suit], fill=color, font=font)
        
        # Symbole central plus grand
        center_font = ImageFont.truetype('arial.ttf', 40) if 'arial.ttf' in font.getname() else font
        w, h = img.size
        draw.text((w//2, h//2), symbols[suit], fill=color, font=center_font, anchor="mm")
        
        # Petits symboles décoratifs pour les figures
        if value > 10:
            for i in range(4):
                x_offset = 20 if i % 2 == 0 else self.CARD_WIDTH - 20
                y_offset = 60 if i < 2 else self.CARD_HEIGHT - 60
                draw.text((x_offset, y_offset), symbols[suit], fill=color, font=small_font, anchor="mm")
        
        return ImageTk.PhotoImage(img)

    def on_click(self, event):
        self.selected = None
        
        # Vérifier le clic sur le stock
        stock_x, stock_y = 50, 50
        if (stock_x <= event.x <= stock_x + self.CARD_WIDTH and 
            stock_y <= event.y <= stock_y + self.CARD_HEIGHT):
            if self.stock:
                self.waste.append(self.stock.pop())
            elif self.waste:
                self.stock = self.waste[::-1]
                self.waste = []
            return
            
        # Vérifier le clic sur la défausse
        waste_x, waste_y = 150, 50
        if (self.waste and waste_x <= event.x <= waste_x + self.CARD_WIDTH and 
            waste_y <= event.y <= waste_y + self.CARD_HEIGHT):
            self.selected = ('waste', len(self.waste) - 1)
            return
            
        # Vérifier le clic sur les fondations
        for i in range(4):
            x = 800 + i * (self.CARD_WIDTH + 10)
            y = 50
            if (x <= event.x <= x + self.CARD_WIDTH and 
                y <= event.y <= y + self.CARD_HEIGHT):
                return  # Juste pour la sélection visuelle, le déplacement se fait dans on_release
                
        # Détection des cartes dans le tableau
        for pile_idx, pile in enumerate(self.tableau):
            if not pile:
                continue
                
            x_pos = 300 + pile_idx * (self.CARD_WIDTH + 10)
            y_pos = 200
            
            for card_idx in range(len(pile)):
                if card_idx < len(pile) - 1:
                    y_pos += 15
                    continue
                    
                if (x_pos <= event.x <= x_pos + self.CARD_WIDTH and
                    y_pos <= event.y <= y_pos + self.CARD_HEIGHT):
                    self.selected = ('tableau', pile_idx, card_idx)
                    return
                    
                y_pos += 30

    def on_drag(self, event):
        if not self.selected:
            return
            
        self.canvas.delete("drag")
        x = event.x - self.CARD_WIDTH//2
        y = event.y - self.CARD_HEIGHT//2
        
        if self.selected[0] == 'waste':
            card = self.waste[self.selected[1]]
            self.canvas.create_image(x, y, image=self.cards[card], anchor="nw", tag="drag")
        elif self.selected[0] == 'tableau':
            _, pile_idx, card_idx = self.selected
            card = self.tableau[pile_idx][card_idx]
            self.canvas.create_image(x, y, image=self.cards[card], anchor="nw", tag="drag")

    def on_release(self, event):
        if not self.selected:
            return
            
        self.canvas.delete("drag")
        
        # Vérifier si on dépose sur une fondation
        for i in range(4):
            x = 800 + i * (self.CARD_WIDTH + 10)
            y = 50
            if (x <= event.x <= x + self.CARD_WIDTH and 
                y <= event.y <= y + self.CARD_HEIGHT):
                # Vérifier si on peut déplacer une carte vers cette fondation
                if self.selected and self.selected[0] == 'waste':
                    card = self.waste[self.selected[1]]
                    if self.can_move_to_foundation(card, i):
                        self.foundation[i].append(card)
                        self.waste.pop()
                        self.selected = None
                        return
                elif self.selected and self.selected[0] == 'tableau':
                    _, pile_idx, card_idx = self.selected
                    if card_idx == len(self.tableau[pile_idx]) - 1:  # Seulement la dernière carte
                        card = self.tableau[pile_idx][card_idx]
                        if self.can_move_to_foundation(card, i):
                            self.foundation[i].append(card)
                            self.tableau[pile_idx].pop()
                            self.selected = None
                            return
        
        # Logique de déplacement selon la source
        if self.selected[0] == 'waste':
            # Déplacer de la défausse vers le tableau
            card = self.waste[self.selected[1]]
            
            # Vérifier les piles du tableau comme cibles
            for target_idx, target_pile in enumerate(self.tableau):
                x_pos = 300 + target_idx * (self.CARD_WIDTH + 10)
                if (x_pos <= event.x <= x_pos + self.CARD_WIDTH and 
                    200 <= event.y <= self.HEIGHT):
                    
                    # Vérifier si le déplacement est valide
                    if (not target_pile and card[1] == 13) or \
                       (target_pile and self.is_valid_move([card], target_pile)):
                        self.tableau[target_idx].append(card)
                        self.waste.pop()
                        break
                        
        elif self.selected[0] == 'tableau':
            _, source_pile, source_idx = self.selected
            moved_cards = self.tableau[source_pile][source_idx:]
            
            # Vérifier les piles du tableau comme cibles
            for target_idx, target_pile in enumerate(self.tableau):
                x_pos = 300 + target_idx * (self.CARD_WIDTH + 10)
                if (x_pos <= event.x <= x_pos + self.CARD_WIDTH and 
                    200 <= event.y <= self.HEIGHT):
                    
                    if target_idx == source_pile:
                        break  # Même pile, pas de déplacement
                        
                    if (not target_pile and moved_cards[0][1] == 13) or \
                       (target_pile and self.is_valid_move(moved_cards, target_pile)):
                        self.tableau[target_idx].extend(moved_cards)
                        del self.tableau[source_pile][source_idx:]
                        break
                        
        self.selected = None

    def can_move_to_foundation(self, card, foundation_idx):
        suit, value = card
        
        # Si la fondation est vide, seul un As peut être placé
        if not self.foundation[foundation_idx]:
            return value == 1  # As
        
        # Sinon, vérifier que c'est la même couleur et la valeur suivante
        top_card = self.foundation[foundation_idx][-1]
        return top_card[0] == suit and top_card[1] == value - 1

    def draw(self):
        self.canvas.delete("all")
        
        # Fond de table amélioré
        self.canvas.create_rectangle(0, 0, self.WIDTH, self.HEIGHT, fill="#2F4F4F")
        
        # Titre du jeu
        self.canvas.create_text(self.WIDTH//2, 20, text="Solitaire", 
                              fill="#FFD700", font=("Arial", 24, "bold"))
        
        # Dessin du stock avec effet 3D
        x, y = 50, 50
        if self.stock:
            # Effet d'empilement pour le stock
            for i in range(min(5, len(self.stock))):
                self.canvas.create_image(x-i, y-i, image=self.card_back, anchor="nw")
            self.canvas.create_image(x, y, image=self.card_back, anchor="nw")
        else:
            self.canvas.create_rectangle(x, y, x+self.CARD_WIDTH, y+self.CARD_HEIGHT, 
                                       outline="#FFD700", dash=(4, 4), width=2)
        
        # Dessin de la défausse avec effet 3D
        x, y = 150, 50
        if self.waste:
            # Effet d'empilement pour la défausse
            for i in range(min(3, len(self.waste)-1)):
                card_idx = len(self.waste) - i - 2
                if card_idx >= 0:
                    self.canvas.create_image(x-i*5, y, image=self.cards[self.waste[card_idx]], anchor="nw")
            self.canvas.create_image(x, y, image=self.cards[self.waste[-1]], anchor="nw")
        else:
            self.canvas.create_rectangle(x, y, x+self.CARD_WIDTH, y+self.CARD_HEIGHT, 
                                       outline="#FFD700", dash=(4, 4), width=2)
        
        # Dessin des fondations avec effet 3D
        for i in range(4):
            x = 800 + i * (self.CARD_WIDTH + 10)
            y = 50
            
            # Cadre de la fondation
            self.canvas.create_rectangle(x-2, y-2, x+self.CARD_WIDTH+2, y+self.CARD_HEIGHT+2, 
                                       outline="#FFD700", width=2)
            
            if self.foundation[i]:
                self.canvas.create_image(x, y, image=self.cards[self.foundation[i][-1]], anchor="nw")
            else:
                suit = ['heart', 'diamond', 'club', 'spade'][i]
                color = "#FF0000" if i < 2 else "#000000"
                self.canvas.create_rectangle(x, y, x+self.CARD_WIDTH, y+self.CARD_HEIGHT, 
                                          fill="#1E3B3B", outline="white")
                self.canvas.create_text(x+self.CARD_WIDTH//2, y+self.CARD_HEIGHT//2, 
                                      text="A♥♦♣♠"[i*2:(i*2)+2], fill=color, font=("Arial", 20))
        
        # Dessin des tableaux avec effet 3D
        for i, pile in enumerate(self.tableau):
            x = 300 + i * (self.CARD_WIDTH + 10)
            y = 200
            
            if not pile:
                self.canvas.create_rectangle(x, y, x+self.CARD_WIDTH, y+self.CARD_HEIGHT, 
                                          outline="#FFD700", dash=(4, 4), width=2)
                continue
            
            # Dessiner les cartes avec un léger décalage pour l'effet 3D
            for idx, card in enumerate(pile):
                is_visible = (idx == len(pile) - 1)
                img = self.cards[card] if is_visible else self.card_back
                
                # Effet d'ombre pour les cartes
                self.canvas.create_rectangle(x+2, y+2, x+self.CARD_WIDTH+2, y+self.CARD_HEIGHT+2, 
                                          fill="#1A2F2F", outline="")
                self.canvas.create_image(x, y, image=img, anchor="nw")
                
                y += 30 if is_visible else 15
        
        # Dessin des fondations
        for i in range(4):
            x = 800 + i * (self.CARD_WIDTH + 10)
            y = 50
            self.canvas.create_rectangle(x, y, x+self.CARD_WIDTH, y+self.CARD_HEIGHT, 
                                       outline="white")

    def setup_bindings(self):
        self.root.bind("<Button-1>", self.on_click)
        self.root.bind("<B1-Motion>", self.on_drag)
        self.root.bind("<ButtonRelease-1>", self.on_release)

    def is_valid_move(self, moved_cards, target_pile):
        if not target_pile:  # Pile vide: seulement un Roi peut être déplacé
            return moved_cards[0][1] == 13  # 13 = King
        
        top_card = target_pile[-1]
        # Vérifier la couleur opposée et valeur décroissante
        color_mapping = {'heart': 'red', 'diamond': 'red', 'club': 'black', 'spade': 'black'}
        moved_color = color_mapping[moved_cards[0][0]]
        target_color = color_mapping[top_card[0]]
        
        return (moved_color != target_color and 
                moved_cards[0][1] == top_card[1] - 1)

    def game_loop(self):
        self.draw()
        self.root.after(100, self.game_loop)

    def reset_game(self):
        self.setup_game()
        self.canvas.delete("all")
        self.game_loop()

if __name__ == "__main__":
    root = tk.Tk()
    game = SolitaireGame(root)
    root.mainloop()
