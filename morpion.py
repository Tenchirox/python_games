import tkinter as tk
from tkinter import messagebox
import random

class MorpionGame:
    def __init__(self, root):
        self.root = root
        self.root.title("Jeu de Morpion")
        self.root.resizable(False, False)
        
        # Variables du jeu
        self.current_player = "X"
        self.board = [""] * 9
        self.game_over = False
        self.mode = "2 joueurs"  # Par défaut
        
        # Création des frames
        self.top_frame = tk.Frame(root)
        self.top_frame.pack(pady=10)
        
        self.board_frame = tk.Frame(root)
        self.board_frame.pack()
        
        self.bottom_frame = tk.Frame(root)
        self.bottom_frame.pack(pady=10)
        
        # Sélection du mode de jeu
        tk.Label(self.top_frame, text="Mode de jeu:").pack(side=tk.LEFT)
        self.mode_var = tk.StringVar(value="2 joueurs")
        modes = ["2 joueurs", "vs Ordinateur"]
        self.mode_menu = tk.OptionMenu(self.top_frame, self.mode_var, *modes, command=self.change_mode)
        self.mode_menu.pack(side=tk.LEFT, padx=10)
        
        # Label pour le joueur actuel
        self.player_label = tk.Label(self.top_frame, text="Joueur actuel: X", font=("Arial", 12))
        self.player_label.pack(side=tk.LEFT, padx=20)
        
        # Création du plateau de jeu
        self.buttons = []
        for i in range(3):
            for j in range(3):
                button = tk.Button(self.board_frame, text="", font=("Arial", 20, "bold"),
                                  width=5, height=2, 
                                  command=lambda row=i, col=j: self.make_move(row, col))
                button.grid(row=i, column=j, padx=5, pady=5)
                self.buttons.append(button)
        
        # Bouton pour redémarrer le jeu
        self.restart_button = tk.Button(self.bottom_frame, text="Nouvelle Partie", 
                                       font=("Arial", 12), command=self.reset_game)
        self.restart_button.pack()
    
    def change_mode(self, mode):
        self.mode = mode
        self.reset_game()
    
    def make_move(self, row, col):
        index = row * 3 + col
        
        # Vérifier si la case est vide et si le jeu n'est pas terminé
        if self.board[index] == "" and not self.game_over:
            # Mettre à jour le plateau
            self.board[index] = self.current_player
            self.buttons[index].config(text=self.current_player, 
                                     fg="blue" if self.current_player == "X" else "red")
            
            # Vérifier s'il y a un gagnant
            if self.check_winner():
                messagebox.showinfo("Fin de partie", f"Le joueur {self.current_player} a gagné!")
                self.game_over = True
                return
            
            # Vérifier s'il y a match nul
            if "" not in self.board:
                messagebox.showinfo("Fin de partie", "Match nul!")
                self.game_over = True
                return
            
            # Changer de joueur
            self.current_player = "O" if self.current_player == "X" else "X"
            self.player_label.config(text=f"Joueur actuel: {self.current_player}")
            
            # Si mode vs ordinateur et c'est au tour de l'ordinateur
            if self.mode == "vs Ordinateur" and self.current_player == "O" and not self.game_over:
                self.root.after(500, self.computer_move)
    
    def computer_move(self):
        # Stratégie simple pour l'ordinateur
        # 1. Gagner si possible
        # 2. Bloquer le joueur s'il peut gagner
        # 3. Jouer au centre si disponible
        # 4. Jouer dans un coin aléatoire
        # 5. Jouer dans une case disponible
        
        # Vérifier si l'ordinateur peut gagner
        for i in range(9):
            if self.board[i] == "":
                self.board[i] = "O"
                if self.check_winner(check_only=True):
                    self.buttons[i].config(text="O", fg="red")
                    self.game_over = True
                    messagebox.showinfo("Fin de partie", "L'ordinateur a gagné!")
                    return
                self.board[i] = ""  # Annuler le mouvement
        
        # Bloquer le joueur s'il peut gagner
        for i in range(9):
            if self.board[i] == "":
                self.board[i] = "X"
                if self.check_winner(check_only=True):
                    self.board[i] = "O"
                    self.buttons[i].config(text="O", fg="red")
                    self.current_player = "X"
                    self.player_label.config(text="Joueur actuel: X")
                    return
                self.board[i] = ""  # Annuler le mouvement
        
        # Jouer au centre si disponible
        if self.board[4] == "":
            self.board[4] = "O"
            self.buttons[4].config(text="O", fg="red")
        else:
            # Jouer dans un coin si disponible
            corners = [0, 2, 6, 8]
            available_corners = [c for c in corners if self.board[c] == ""]
            
            if available_corners:
                move = random.choice(available_corners)
                self.board[move] = "O"
                self.buttons[move].config(text="O", fg="red")
            else:
                # Jouer dans n'importe quelle case disponible
                available_moves = [i for i in range(9) if self.board[i] == ""]
                if available_moves:
                    move = random.choice(available_moves)
                    self.board[move] = "O"
                    self.buttons[move].config(text="O", fg="red")
        
        # Vérifier si l'ordinateur a gagné
        if self.check_winner():
            messagebox.showinfo("Fin de partie", "L'ordinateur a gagné!")
            self.game_over = True
            return
        
        # Vérifier s'il y a match nul
        if "" not in self.board:
            messagebox.showinfo("Fin de partie", "Match nul!")
            self.game_over = True
            return
            
        # Passer au joueur
        self.current_player = "X"
        self.player_label.config(text="Joueur actuel: X")
    
    def check_winner(self, check_only=False):
        # Combinaisons gagnantes
        win_combinations = [
            [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Lignes
            [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Colonnes
            [0, 4, 8], [2, 4, 6]              # Diagonales
        ]
        
        current = self.current_player
        for combo in win_combinations:
            if self.board[combo[0]] == self.board[combo[1]] == self.board[combo[2]] == current:
                if not check_only:
                    # Mettre en surbrillance la combinaison gagnante
                    for idx in combo:
                        self.buttons[idx].config(bg="light green")
                return True
        return False
    
    def reset_game(self):
        # Réinitialiser le plateau
        self.board = [""] * 9
        for button in self.buttons:
            button.config(text="", bg="SystemButtonFace")
        
        # Réinitialiser les variables du jeu
        self.current_player = "X"
        self.player_label.config(text="Joueur actuel: X")
        self.game_over = False
        
        # Si mode vs ordinateur et le joueur est O, l'ordinateur commence
        if self.mode == "vs Ordinateur" and self.current_player == "O":
            self.computer_move()

if __name__ == "__main__":
    root = tk.Tk()
    game = MorpionGame(root)
    root.mainloop()