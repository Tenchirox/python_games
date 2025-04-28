import tkinter as tk
from tkinter import messagebox
import random
import os
import time
from PIL import Image, ImageTk, ImageDraw

class AI_Player:
    def __init__(self, game, player_number=2):
        self.game = game
        self.player_number = player_number  # Par défaut, l'IA est le joueur 2
        self.opponent = 3 - player_number  # L'adversaire est l'autre joueur
    
    def get_best_move(self):
        # Évaluer chaque colonne possible
        best_score = -float('inf')
        best_col = 0
        valid_moves = []
        
        # Trouver toutes les colonnes valides
        for col in range(self.game.COLS):
            if self.game.board[0][col] == 0:  # Si la colonne n'est pas pleine
                valid_moves.append(col)
        
        # Si aucun coup valide, retourner None
        if not valid_moves:
            return None
        
        # Évaluer chaque coup possible
        for col in valid_moves:
            # Trouver la position où le jeton va tomber
            row = self.game.ROWS - 1
            while row >= 0 and self.game.board[row][col] != 0:
                row -= 1
            
            # Simuler le coup
            self.game.board[row][col] = self.player_number
            
            # Évaluer le plateau après ce coup
            score = self.evaluate_board(row, col)
            
            # Annuler le coup
            self.game.board[row][col] = 0
            
            # Mettre à jour le meilleur coup si nécessaire
            if score > best_score:
                best_score = score
                best_col = col
        
        return best_col
    
    def evaluate_board(self, last_row, last_col):
        score = 0
        player = self.player_number
        opponent = self.opponent
        
        # Vérifier si ce coup est gagnant
        if self.game.check_winner(last_row, last_col):
            return 1000  # Score très élevé pour un coup gagnant
        
        # Vérifier si l'adversaire peut gagner au prochain tour
        for col in range(self.game.COLS):
            if self.game.board[0][col] != 0:  # Colonne pleine
                continue
                
            # Trouver où le jeton va tomber
            row = self.game.ROWS - 1
            while row >= 0 and self.game.board[row][col] != 0:
                row -= 1
                
            # Simuler le coup de l'adversaire
            self.game.board[row][col] = opponent
            
            # Si l'adversaire peut gagner, bloquer cette colonne
            if self.game.check_winner(row, col):
                score -= 500  # Score négatif élevé
                
            # Annuler le coup
            self.game.board[row][col] = 0
        
        # Favoriser le centre
        center_col = self.game.COLS // 2
        if last_col == center_col:
            score += 3
        
        # Favoriser les positions qui permettent de créer des alignements
        # Horizontalement
        for c in range(max(0, last_col-3), min(self.game.COLS-3, last_col+1)):
            window = [self.game.board[last_row][c+i] for i in range(4)]
            score += self.evaluate_window(window)
        
        # Verticalement
        for r in range(max(0, last_row-3), min(self.game.ROWS-3, last_row+1)):
            window = [self.game.board[r+i][last_col] for i in range(4)]
            score += self.evaluate_window(window)
        
        # Diagonale (haut-gauche à bas-droite)
        for i in range(-3, 1):
            if 0 <= last_row+i < self.game.ROWS-3 and 0 <= last_col+i < self.game.COLS-3:
                window = [self.game.board[last_row+i+j][last_col+i+j] for j in range(4)]
                score += self.evaluate_window(window)
        
        # Diagonale (bas-gauche à haut-droite)
        for i in range(-3, 1):
            if 3 <= last_row+i < self.game.ROWS and 0 <= last_col+i < self.game.COLS-3:
                window = [self.game.board[last_row+i-j][last_col+i+j] for j in range(4)]
                score += self.evaluate_window(window)
        
        return score
    
    def evaluate_window(self, window):
        score = 0
        player_count = window.count(self.player_number)
        empty_count = window.count(0)
        opponent_count = window.count(self.opponent)
        
        # Évaluer la fenêtre
        if player_count == 3 and empty_count == 1:
            score += 5  # Trois jetons alignés avec une case vide
        elif player_count == 2 and empty_count == 2:
            score += 2  # Deux jetons alignés avec deux cases vides
        
        # Pénaliser les fenêtres où l'adversaire a des jetons
        if opponent_count == 3 and empty_count == 1:
            score -= 4  # Bloquer l'adversaire qui a trois jetons alignés
        
        return score

class Puissance4Game:
    def __init__(self, root):
        self.root = root
        self.root.title("Puissance 4")
        self.root.resizable(False, False)
        
        # Constantes du jeu
        self.WIDTH = 700
        self.HEIGHT = 600
        self.ROWS = 6  # Nombre de lignes
        self.COLS = 7  # Nombre de colonnes
        self.CELL_SIZE = 80  # Taille d'une cellule
        self.ANIMATION_SPEED = 20  # Vitesse de chute des jetons
        
        # Variables du jeu
        self.board = [[0 for _ in range(self.COLS)] for _ in range(self.ROWS)]
        self.current_player = 1  # Joueur 1 commence
        self.game_over = False
        self.game_started = False
        self.scores = {1: 0, 2: 0}  # Scores des joueurs
        self.falling_piece = None  # Pour l'animation de chute
        self.ai_enabled = False  # Par défaut, pas d'IA
        self.ai_player = None  # Instance de l'IA
        
        # Création du canvas principal
        self.canvas = tk.Canvas(root, width=self.WIDTH, height=self.HEIGHT, bg="#2F2F2F")
        self.canvas.pack(side=tk.LEFT)
        
        # Panneau d'information
        self.info_frame = tk.Frame(root, width=200, height=self.HEIGHT, bg="#1F1F1F")
        self.info_frame.pack(side=tk.RIGHT, fill=tk.BOTH)
        
        # Affichage des scores
        self.player1_label = tk.Label(self.info_frame, text="Joueur 1: 0", font=("Arial", 14), bg="#1F1F1F", fg="#FF0000")
        self.player1_label.pack(pady=10)
        
        self.player2_label = tk.Label(self.info_frame, text="Joueur 2: 0", font=("Arial", 14), bg="#1F1F1F", fg="#FFFF00")
        self.player2_label.pack(pady=10)
        
        self.turn_label = tk.Label(self.info_frame, text="Tour: Joueur 1", font=("Arial", 14), bg="#1F1F1F", fg="white")
        self.turn_label.pack(pady=10)
        
        # Option pour activer/désactiver l'IA
        self.ai_frame = tk.Frame(self.info_frame, bg="#1F1F1F")
        self.ai_frame.pack(pady=10)
        
        self.ai_var = tk.BooleanVar(value=False)
        self.ai_checkbox = tk.Checkbutton(self.ai_frame, text="Jouer contre l'IA", 
                                        variable=self.ai_var, 
                                        font=("Arial", 12),
                                        bg="#1F1F1F", fg="white",
                                        selectcolor="#3F3F3F",
                                        command=self.toggle_ai)
        self.ai_checkbox.pack(side=tk.LEFT)
        
        # Boutons de contrôle
        self.start_button = tk.Button(self.info_frame, text="Commencer", 
                                    font=("Arial", 12), command=self.start_game)
        self.start_button.pack(pady=10)
        
        self.restart_button = tk.Button(self.info_frame, text="Nouvelle Partie", 
                                      font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack(pady=10)
        self.restart_button.pack_forget()  # Cacher le bouton au démarrage
        
        # Chargement des sprites
        self.load_sprites()
        
        # Liaison des événements
        self.canvas.bind("<Motion>", self.on_mouse_move)
        self.canvas.bind("<Button-1>", self.on_click)
        
        # Afficher l'écran d'accueil
        self.show_welcome_screen()
    
    def load_sprites(self):
        # Créer un dossier pour les sprites s'il n'existe pas
        sprites_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sprites")
        if not os.path.exists(sprites_dir):
            os.makedirs(sprites_dir)
        
        # Créer les sprites pour les jetons
        self.sprites = {}
        
        # Jeton rouge (joueur 1)
        self.sprites["player1"] = self.create_token_sprite("#FF0000")
        
        # Jeton jaune (joueur 2)
        self.sprites["player2"] = self.create_token_sprite("#FFFF00")
        
        # Case vide
        self.sprites["empty"] = self.create_empty_sprite()
    
    def create_token_sprite(self, color):
        # Créer un sprite pour un jeton
        size = self.CELL_SIZE - 10
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner le jeton (cercle avec effet 3D)
        draw.ellipse([0, 0, size-1, size-1], fill=color)
        
        # Effet de brillance
        highlight_size = size // 3
        draw.ellipse([size//4, size//4, size//4 + highlight_size, size//4 + highlight_size], 
                     fill="white", outline=None)
        
        return ImageTk.PhotoImage(img)
    
    def create_empty_sprite(self):
        # Créer un sprite pour une case vide
        size = self.CELL_SIZE - 10
        img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Dessiner un cercle vide avec bordure
        draw.ellipse([0, 0, size-1, size-1], fill="#1F1F1F", outline="#3F3F3F", width=2)
        
        return ImageTk.PhotoImage(img)
    
    def show_welcome_screen(self):
        # Afficher un écran d'accueil avec des instructions
        self.canvas.delete("all")
        
        # Titre du jeu
        self.canvas.create_text(self.WIDTH // 2, self.HEIGHT // 4, 
                              text="PUISSANCE 4", 
                              font=("Arial", 36, "bold"), 
                              fill="white")
        
        # Instructions
        instructions = [
            "Alignez 4 jetons de votre couleur pour gagner",
            "Cliquez sur une colonne pour y placer votre jeton",
            "Les jetons tombent jusqu'à la position la plus basse",
            "Le premier joueur utilise des jetons rouges",
            "Le deuxième joueur utilise des jetons jaunes",
            "\nCliquez sur 'Commencer' pour jouer"
        ]
        
        y_pos = self.HEIGHT // 2
        for line in instructions:
            self.canvas.create_text(self.WIDTH // 2, y_pos, 
                                  text=line, 
                                  font=("Arial", 14), 
                                  fill="white")
            y_pos += 30
        
        # Dessiner quelques jetons décoratifs
        self.canvas.create_image(self.WIDTH // 3, self.HEIGHT * 3 // 4, 
                               image=self.sprites["player1"], anchor="center")
        
        self.canvas.create_image(self.WIDTH * 2 // 3, self.HEIGHT * 3 // 4, 
                               image=self.sprites["player2"], anchor="center")
    
    def toggle_ai(self):
        # Activer ou désactiver l'IA
        self.ai_enabled = self.ai_var.get()
        if self.ai_enabled:
            self.ai_player = AI_Player(self)
        else:
            self.ai_player = None
    
    def start_game(self):
        # Démarrer le jeu
        self.game_started = True
        self.game_over = False
        
        # Changer l'apparence des boutons
        self.start_button.pack_forget()
        self.restart_button.pack(pady=10)
        
        # Réinitialiser le jeu
        self.reset_game()
        
        # Dessiner le plateau
        self.draw_board()
        
        # Initialiser l'IA si activée
        if self.ai_enabled:
            self.ai_player = AI_Player(self)
    
    def reset_game(self):
        # Réinitialiser les variables du jeu
        self.board = [[0 for _ in range(self.COLS)] for _ in range(self.ROWS)]
        self.current_player = 1
        self.game_over = False
        self.falling_piece = None
        
        # Mettre à jour les labels
        self.turn_label.config(text="Tour: Joueur 1")
        
        # Réinitialiser l'IA si activée
        if self.ai_enabled:
            self.ai_player = AI_Player(self)
        
        # Dessiner le plateau
        self.draw_board()
    
    def draw_board(self):
        # Dessiner le plateau de jeu
        self.canvas.delete("all")
        
        # Dessiner le fond du plateau
        self.canvas.create_rectangle(0, 0, self.COLS * self.CELL_SIZE, 
                                   self.ROWS * self.CELL_SIZE, 
                                   fill="#0000AA", outline="")
        
        # Dessiner les cases
        for row in range(self.ROWS):
            for col in range(self.COLS):
                x = col * self.CELL_SIZE + self.CELL_SIZE // 2
                y = row * self.CELL_SIZE + self.CELL_SIZE // 2
                
                if self.board[row][col] == 0:
                    self.canvas.create_image(x, y, image=self.sprites["empty"])
                elif self.board[row][col] == 1:
                    self.canvas.create_image(x, y, image=self.sprites["player1"])
                elif self.board[row][col] == 2:
                    self.canvas.create_image(x, y, image=self.sprites["player2"])
        
        # Dessiner le jeton en cours de chute (animation)
        if self.falling_piece:
            col, y, player = self.falling_piece
            x = col * self.CELL_SIZE + self.CELL_SIZE // 2
            sprite_key = "player1" if player == 1 else "player2"
            self.canvas.create_image(x, y, image=self.sprites[sprite_key])
        
        # Dessiner la zone de prévisualisation
        if not self.game_over and self.game_started:
            col = self.get_column_from_x(self.canvas.winfo_pointerx() - self.canvas.winfo_rootx())
            if 0 <= col < self.COLS:
                x = col * self.CELL_SIZE + self.CELL_SIZE // 2
                sprite_key = "player1" if self.current_player == 1 else "player2"
                self.canvas.create_image(x, self.CELL_SIZE // 2, image=self.sprites[sprite_key], 
                                       state="disabled", tags="preview")
    
    def get_column_from_x(self, x):
        # Convertir la position x en numéro de colonne
        return x // self.CELL_SIZE
    
    def on_mouse_move(self, event):
        # Gérer le mouvement de la souris pour la prévisualisation
        if self.game_started and not self.game_over and not self.falling_piece:
            self.draw_board()
    
    def on_click(self, event):
        # Gérer le clic pour placer un jeton
        if not self.game_started or self.game_over or self.falling_piece:
            return
        
        col = self.get_column_from_x(event.x)
        if 0 <= col < self.COLS:
            self.drop_piece(col)
    
    def drop_piece(self, col):
        # Vérifier si la colonne est pleine
        if self.board[0][col] != 0:
            return
        
        # Trouver la position la plus basse disponible
        row = self.ROWS - 1
        while row >= 0 and self.board[row][col] != 0:
            row -= 1
        
        # Animer la chute du jeton
        self.falling_piece = [col, 0, self.current_player]
        self.animate_falling_piece(row, col)
    
    def animate_falling_piece(self, target_row, col):
        # Animer la chute d'un jeton
        _, y, player = self.falling_piece
        target_y = target_row * self.CELL_SIZE + self.CELL_SIZE // 2
        
        if y < target_y:
            # Mettre à jour la position du jeton
            y += 10  # Vitesse de chute
            self.falling_piece = [col, y, player]
            self.draw_board()
            self.root.after(self.ANIMATION_SPEED, lambda: self.animate_falling_piece(target_row, col))
        else:
            # Placer le jeton sur le plateau
            self.board[target_row][col] = player
            self.falling_piece = None
            self.draw_board()
            
            # Vérifier s'il y a un gagnant
            if self.check_winner(target_row, col):
                self.game_over = True
                self.scores[player] += 1
                self.update_score_labels()
                winner = "Joueur 1" if player == 1 else "Joueur 2"
                messagebox.showinfo("Partie terminée", f"{winner} a gagné!")
            elif self.is_board_full():
                self.game_over = True
                messagebox.showinfo("Partie terminée", "Match nul!")
            else:
                # Passer au joueur suivant
                self.current_player = 3 - self.current_player  # Alterne entre 1 et 2
                self.turn_label.config(text=f"Tour: Joueur {self.current_player}")
                
                # Si c'est au tour de l'IA et qu'elle est activée
                if self.ai_enabled and self.current_player == 2 and not self.game_over:
                    self.root.after(500, self.ai_make_move)  # Délai pour que l'IA joue
    
    def update_score_labels(self):
        # Mettre à jour les labels de score
        self.player1_label.config(text=f"Joueur 1: {self.scores[1]}")
        self.player2_label.config(text=f"Joueur 2: {self.scores[2]}")
    
    def check_winner(self, row, col):
        # Vérifier s'il y a un gagnant
        player = self.board[row][col]
        
        # Vérifier horizontalement
        count = 0
        for c in range(self.COLS):
            if self.board[row][c] == player:
                count += 1
                if count >= 4:
                    return True
            else:
                count = 0
        
        # Vérifier verticalement
        count = 0
        for r in range(self.ROWS):
            if self.board[r][col] == player:
                count += 1
                if count >= 4:
                    return True
            else:
                count = 0
        
        # Vérifier diagonale (haut-gauche à bas-droite)
        # Trouver le point de départ de la diagonale
        start_row, start_col = row, col
        while start_row > 0 and start_col > 0:
            start_row -= 1
            start_col -= 1
        
        # Parcourir la diagonale
        count = 0
        r, c = start_row, start_col
        while r < self.ROWS and c < self.COLS:
            if self.board[r][c] == player:
                count += 1
                if count >= 4:
                    return True
            else:
                count = 0
            r += 1
            c += 1
        
        # Vérifier diagonale (bas-gauche à haut-droite)
        # Trouver le point de départ de la diagonale
        start_row, start_col = row, col
        while start_row < self.ROWS - 1 and start_col > 0:
            start_row += 1
            start_col -= 1
        
        # Parcourir la diagonale
        count = 0
        r, c = start_row, start_col
        while r >= 0 and c < self.COLS:
            if self.board[r][c] == player:
                count += 1
                if count >= 4:
                    return True
            else:
                count = 0
            r -= 1
            c += 1
        
        return False
    
    def is_board_full(self):
        # Vérifier si le plateau est plein
        return all(self.board[0][col] != 0 for col in range(self.COLS))
    
    def ai_make_move(self):
        # L'IA joue son coup
        if self.game_over or self.falling_piece:
            return
        
        # Obtenir le meilleur coup selon l'IA
        best_col = self.ai_player.get_best_move()
        
        # Jouer le coup
        if best_col is not None:
            self.drop_piece(best_col)

# Fonction principale
def main():
    root = tk.Tk()
    game = Puissance4Game(root)
    root.mainloop()

if __name__ == "__main__":
    main()