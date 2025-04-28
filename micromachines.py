import pygame
import sys
import math
import random

# Initialisation de Pygame
pygame.init()

# Constantes
WIDTH, HEIGHT = 800, 600
FPS = 60

# Couleurs
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
GRAY = (100, 100, 100)
YELLOW = (255, 255, 0)
ORANGE = (255, 165, 0)
PURPLE = (128, 0, 128)
CYAN = (0, 255, 255)

# États du jeu
MENU = 0
PLAYING = 1
GAME_OVER = 2
LEVEL_COMPLETE = 3

# Création de la fenêtre
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("MicroMachines Python")
clock = pygame.time.Clock()

class Car:
    def __init__(self, x, y, color=RED, highlight=YELLOW, angle=0):
        self.x = x
        self.y = y
        self.width = 30
        self.height = 15
        self.angle = angle  # Angle en degrés
        self.speed = 0
        self.max_speed = 3  # Réduit de 5 à 4
        self.acceleration = 0.04  # Réduit de 0.1 à 0.08
        self.deceleration = 0.04  # Réduit de 0.05 à 0.04
        self.rotation_speed = 3
        self.drift_factor = 0.95  # Facteur de dérive
        self.lap_progress = 0  # Progression sur le circuit (0-100%)
        self.checkpoint_index = 0  # Index du prochain checkpoint
        self.laps_completed = 0  # Nombre de tours complétés
        
        # Création d'une surface pour la voiture
        self.original_image = pygame.Surface((self.width, self.height), pygame.SRCALPHA)
        pygame.draw.rect(self.original_image, color, (0, 0, self.width, self.height))
        pygame.draw.rect(self.original_image, highlight, (self.width-5, 3, 5, self.height-6))
        self.image = self.original_image
        self.rect = self.image.get_rect(center=(self.x, self.y))
        
    def update(self, keys):
        # Accélération avec la touche ESPACE
        if keys[pygame.K_SPACE]:
            self.speed += self.acceleration
            if self.speed > self.max_speed:
                self.speed = self.max_speed
        # Freinage avec la touche ALT
        elif keys[pygame.K_LALT] or keys[pygame.K_RALT]:
            self.speed -= self.deceleration * 2
            if self.speed < -self.max_speed / 2:  # Marche arrière limitée
                self.speed = -self.max_speed / 2
        # Décélération naturelle
        else:
            if self.speed > 0:
                self.speed -= self.deceleration
                if self.speed < 0:
                    self.speed = 0
            elif self.speed < 0:
                self.speed += self.deceleration
                if self.speed > 0:
                    self.speed = 0
        
        # Rotation gauche/droite seulement si la voiture est en mouvement
        if abs(self.speed) > 0.1:
            if keys[pygame.K_LEFT]:
                self.angle += self.rotation_speed * (self.speed / self.max_speed)
            if keys[pygame.K_RIGHT]:
                self.angle -= self.rotation_speed * (self.speed / self.max_speed)
        
        # Calcul du mouvement en fonction de l'angle
        angle_rad = math.radians(self.angle)
        dx = -math.sin(angle_rad) * self.speed
        dy = math.cos(angle_rad) * self.speed
        
        # Application du mouvement
        self.x += dx
        self.y += dy
        
        # Limites de l'écran
        if self.x < 0:
            self.x = 0
            self.speed *= 0.5  # Rebond
        elif self.x > WIDTH:
            self.x = WIDTH
            self.speed *= 0.5  # Rebond
        if self.y < 0:
            self.y = 0
            self.speed *= 0.5  # Rebond
        elif self.y > HEIGHT:
            self.y = HEIGHT
            self.speed *= 0.5  # Rebond
        
        # Mise à jour de l'image et du rectangle
        self.image = pygame.transform.rotate(self.original_image, self.angle)
        self.rect = self.image.get_rect(center=(self.x, self.y))
    
    def draw(self, surface):
        surface.blit(self.image, self.rect.topleft)

class Track:
    def __init__(self, level=1):
        self.width = WIDTH
        self.height = HEIGHT
        self.track_surface = pygame.Surface((self.width, self.height))
        self.level = level
        self.checkpoints = []
        self.start_position = (0, 0)
        self.start_angle = 0
        
        # Créer le circuit en fonction du niveau
        self.create_track(level)
    
    def create_track(self, level):
        self.track_surface.fill(GREEN)  # Fond vert (herbe)
        
        if level == 1:  # Circuit rectangulaire simple
            self.track_points = [
                (100, 100), (700, 100), (700, 500), (100, 500), (100, 100)
            ]
            self.start_position = (200, 100)
            self.start_angle = 0
            # Points de passage pour l'IA
            self.checkpoints = [
                (400, 100), (700, 300), (400, 500), (100, 300)
            ]
            
        elif level == 2:  # Circuit en forme de 8
            self.track_points = [
                (100, 150), (350, 100), (650, 150), (700, 300),
                (650, 450), (350, 500), (100, 450), (50, 300),
                (100, 150)
            ]
            self.start_position = (100, 200)
            self.start_angle = 0
            # Points de passage pour l'IA
            self.checkpoints = [
                (350, 100), (700, 300), (350, 500), (50, 300)
            ]
            
        elif level == 3:  # Circuit complexe
            self.track_points = [
                (100, 100), (300, 50), (500, 100), (700, 200),
                (650, 350), (700, 500), (500, 550), (300, 500),
                (100, 450), (50, 300), (100, 100)
            ]
            self.start_position = (100, 150)
            self.start_angle = 0
            # Points de passage pour l'IA
            self.checkpoints = [
                (300, 50), (700, 200), (650, 350), (700, 500),
                (300, 500), (50, 300)
            ]
        
        # Dessiner la piste (route)
        pygame.draw.lines(self.track_surface, GRAY, True, self.track_points, 60)
        
        # Ligne de départ/arrivée
        start_x, start_y = self.start_position
        pygame.draw.line(self.track_surface, WHITE, (start_x, start_y-30), (start_x, start_y+30), 5)
        
        # Obstacles
        self.obstacles = []
        num_obstacles = 3 + level * 2  # Plus d'obstacles avec les niveaux supérieurs
        for _ in range(num_obstacles):
            valid_position = False
            while not valid_position:
                x = random.randint(50, WIDTH-50)
                y = random.randint(50, HEIGHT-50)
                # Vérifier si l'obstacle est sur la piste
                if self.track_surface.get_at((x, y)) == GRAY:
                    size = random.randint(10, 20)
                    self.obstacles.append(pygame.Rect(x, y, size, size))
                    pygame.draw.rect(self.track_surface, BLUE, (x, y, size, size))
                    valid_position = True
    
    def draw(self, surface):
        surface.blit(self.track_surface, (0, 0))
    
    def check_collision(self, car_rect):
        # Vérifier si la voiture est sur la piste (zone grise)
        car_center = car_rect.center
        
        # Convertir les coordonnées en entiers et s'assurer qu'elles sont dans les limites
        car_center_x = max(0, min(int(car_center[0]), self.width - 1))
        car_center_y = max(0, min(int(car_center[1]), self.height - 1))
        car_center_int = (car_center_x, car_center_y)
        
        # Vérifier si les coordonnées originales sont hors limites
        if not (0 <= int(car_center[0]) < self.width and 0 <= int(car_center[1]) < self.height):
            return True  # Considérer hors piste si hors limites
        
        try:
            track_color = self.track_surface.get_at(car_center_int)
            
            # Si la voiture n'est pas sur la piste (pas gris), elle est sur l'herbe
            if track_color != GRAY:
                return True
            
            # Vérifier les collisions avec les obstacles
            for obstacle in self.obstacles:
                if car_rect.colliderect(obstacle):
                    return True
            
            return False
        except IndexError:
            # En cas d'erreur d'index, considérer comme hors piste
            return True

class AI_Car(Car):
    def __init__(self, x, y, track, color=ORANGE, highlight=WHITE, difficulty=1, angle=0):
        super().__init__(x, y, color, highlight, angle)
        self.track = track
        self.target_checkpoint = 0
        self.difficulty = difficulty  # 1-3: facile, moyen, difficile
        
        # Ajuster les caractéristiques en fonction de la difficulté
        if difficulty == 1:  # Facile
            self.max_speed = 2.5
            self.acceleration = 0.03
            self.rotation_speed = 2.5
        elif difficulty == 2:  # Moyen
            self.max_speed = 2.8
            self.acceleration = 0.035
            self.rotation_speed = 2.8
        else:  # Difficile
            self.max_speed = 3.0
            self.acceleration = 0.04
            self.rotation_speed = 3.0
    
    def update(self, _=None):  # Ignorer les touches, l'IA contrôle la voiture
        # Trouver le checkpoint cible
        if self.target_checkpoint >= len(self.track.checkpoints):
            self.target_checkpoint = 0
            self.laps_completed += 1
        
        # Obtenir les coordonnées du checkpoint actuel
        target_x, target_y = self.track.checkpoints[self.target_checkpoint]
        
        # Vérifier si la voiture est sur la piste ou sur l'herbe en utilisant la méthode check_collision
        on_track = not self.track.check_collision(self.rect)
        
        # Calculer l'angle vers le checkpoint actuel
        dx = target_x - self.x
        dy = target_y - self.y
        target_angle = math.degrees(math.atan2(-dx, dy)) % 360
        
        # Calculer la différence d'angle
        angle_diff = (target_angle - self.angle) % 360
        if angle_diff > 180:
            angle_diff -= 360
        
        # Déterminer le prochain checkpoint et le précédent pour une meilleure navigation
        next_checkpoint = (self.target_checkpoint + 1) % len(self.track.checkpoints)
        prev_checkpoint = (self.target_checkpoint - 1) % len(self.track.checkpoints)
        next_x, next_y = self.track.checkpoints[next_checkpoint]
        prev_x, prev_y = self.track.checkpoints[prev_checkpoint]
        
        # Créer des points intermédiaires sur la piste pour mieux suivre le tracé
        # Cela aide à éviter de couper à travers l'herbe
        distance_to_current = math.sqrt(dx**2 + dy**2)
        
        # Si on est sur l'herbe, priorité à revenir sur la piste
        if not on_track:
            # Chercher le point le plus proche sur la piste
            found_track = False
            closest_track_point = None
            closest_distance = float('inf')
            
            # Tester plusieurs rayons pour trouver la piste
            for test_distance in [30, 60, 90, 120]:
                if found_track:
                    break
                for test_angle in range(0, 360, 20):  # Tester 18 directions (plus précis)
                    test_rad = math.radians(test_angle)
                    test_x = self.x + math.cos(test_rad) * test_distance
                    test_y = self.y + math.sin(test_rad) * test_distance
                    test_rect = pygame.Rect(test_x-5, test_y-5, 10, 10)
                    
                    if not self.track.check_collision(test_rect):  # Si ce point est sur la piste
                        # Calculer la distance à ce point
                        point_distance = math.sqrt((test_x - self.x)**2 + (test_y - self.y)**2)
                        if point_distance < closest_distance:
                            closest_distance = point_distance
                            closest_track_point = (test_x, test_y)
                            found_track = True
            
            # Si on a trouvé un point sur la piste, se diriger vers lui
            if found_track and closest_track_point:
                target_x, target_y = closest_track_point
                dx = target_x - self.x
                dy = target_y - self.y
                target_angle = math.degrees(math.atan2(-dx, dy)) % 360
                angle_diff = (target_angle - self.angle) % 360
                if angle_diff > 180:
                    angle_diff -= 360
                # Forcer un ralentissement important sur l'herbe
                self.speed = min(self.speed, self.max_speed * 0.4)
        # Si on est sur la piste et proche du checkpoint actuel, anticiper le prochain
        elif distance_to_current < 100:  # Si on est à moins de 100 pixels du checkpoint actuel
            # Calculer un point intermédiaire entre le checkpoint actuel et le prochain
            # Plus on est proche du checkpoint actuel, plus on vise le prochain
            weight = max(0, min(1, (100 - distance_to_current) / 100))
            
            # Créer un point intermédiaire qui reste sur la piste
            # Au lieu de viser directement le prochain checkpoint (ce qui pourrait couper par l'herbe)
            # on crée un point qui suit la courbure de la piste
            
            # Calculer le vecteur de direction actuel
            current_to_next = (next_x - target_x, next_y - target_y)
            
            # Normaliser le vecteur
            length = math.sqrt(current_to_next[0]**2 + current_to_next[1]**2)
            if length > 0:
                current_to_next = (current_to_next[0]/length, current_to_next[1]/length)
            
            # Créer un point intermédiaire qui suit la courbure de la piste
            intermediate_x = target_x + current_to_next[0] * distance_to_current * weight
            intermediate_y = target_y + current_to_next[1] * distance_to_current * weight
            
            # Vérifier si ce point intermédiaire est sur la piste
            test_rect = pygame.Rect(intermediate_x-5, intermediate_y-5, 10, 10)
            if not self.track.check_collision(test_rect):
                target_x = intermediate_x
                target_y = intermediate_y
            else:
                # Si le point intermédiaire n'est pas sur la piste, utiliser une approche plus simple
                target_x = target_x * (1 - weight) + next_x * weight
                target_y = target_y * (1 - weight) + next_y * weight
            
            # Recalculer l'angle vers ce point intermédiaire
            dx = target_x - self.x
            dy = target_y - self.y
            target_angle = math.degrees(math.atan2(-dx, dy)) % 360
            
            # Recalculer la différence d'angle
            angle_diff = (target_angle - self.angle) % 360
            if angle_diff > 180:
                angle_diff -= 360
        
        # Tourner vers le checkpoint avec une rotation plus précise
        if abs(angle_diff) > 5:  # Marge d'erreur
            # Ajuster la vitesse de rotation en fonction de l'angle
            turn_speed = self.rotation_speed * min(1.0, self.speed / self.max_speed)
            
            # Tourner plus rapidement si l'angle est grand
            turn_multiplier = min(2.0, max(1.0, abs(angle_diff) / 45))
            
            if angle_diff > 0:
                self.angle += turn_speed * turn_multiplier
            else:
                self.angle -= turn_speed * turn_multiplier
        
        # Accélérer si on est dans la bonne direction et sur la piste, freiner sinon
        if abs(angle_diff) < 45 and on_track:  # Si on est à peu près dans la bonne direction et sur la piste
            self.speed += self.acceleration
            if self.speed > self.max_speed:
                self.speed = self.max_speed
        else:  # Sinon, on ralentit
            # Ralentir davantage si on est sur l'herbe ou si l'angle est mauvais
            decel_factor = 1.0
            if not on_track:
                decel_factor = 5.0  # Pénalité beaucoup plus forte sur l'herbe
                self.speed *= 0.85  # Ralentissement supplémentaire sur l'herbe
                
                # Si on est sur l'herbe, priorité absolue: revenir sur la piste
                # Chercher la direction de la piste la plus proche
                found_track = False
                for test_distance in [30, 60, 90]:  # Tester à différentes distances
                    if found_track:
                        break
                    for test_angle in range(0, 360, 30):  # Tester 12 directions (plus précis)
                        test_rad = math.radians(test_angle)
                        test_x = self.x + math.cos(test_rad) * test_distance
                        test_y = self.y + math.sin(test_rad) * test_distance
                        test_rect = pygame.Rect(test_x-5, test_y-5, 10, 10)
                        
                        if not self.track.check_collision(test_rect):  # Si ce point est sur la piste
                            # Ajuster l'angle pour se diriger vers ce point avec priorité absolue
                            target_angle = math.degrees(math.atan2(-(test_x - self.x), (test_y - self.y))) % 360
                            angle_diff = (target_angle - self.angle) % 360
                            if angle_diff > 180:
                                angle_diff -= 360
                            found_track = True
                            break
                
                # Si aucun chemin vers la piste n'est trouvé, revenir vers le dernier checkpoint
                if not found_track:
                    prev_checkpoint = (self.target_checkpoint - 1) % len(self.track.checkpoints)
                    prev_x, prev_y = self.track.checkpoints[prev_checkpoint]
                    target_angle = math.degrees(math.atan2(-(prev_x - self.x), (prev_y - self.y))) % 360
                    angle_diff = (target_angle - self.angle) % 360
                    if angle_diff > 180:
                        angle_diff -= 360
            elif abs(angle_diff) > 90:
                decel_factor = 2.0  # Pénalité si on va dans la mauvaise direction
                
            self.speed -= self.deceleration * decel_factor
            if self.speed < 0.5:  # Vitesse minimale pour ne pas s'arrêter complètement
                self.speed = 0.5
        
        # Vérifier si on a atteint le checkpoint
        distance = math.sqrt(dx**2 + dy**2)
        if distance < 50:  # Rayon du checkpoint
            self.target_checkpoint += 1
            
            # Ajuster la vitesse après avoir atteint un checkpoint
            if on_track:
                # Accélération bonus quand on atteint un checkpoint sur la piste
                self.speed = min(self.max_speed, self.speed + 0.5)
            else:
                # Ralentir si on a atteint le checkpoint en étant hors piste
                self.speed *= 0.7
        
        # Calcul du mouvement en fonction de l'angle
        angle_rad = math.radians(self.angle)
        dx = -math.sin(angle_rad) * self.speed
        dy = math.cos(angle_rad) * self.speed
        
        # Application du mouvement
        self.x += dx
        self.y += dy
        
        # Limites de l'écran
        if self.x < 0:
            self.x = 0
            self.speed *= 0.5  # Rebond
        elif self.x > WIDTH:
            self.x = WIDTH
            self.speed *= 0.5  # Rebond
        if self.y < 0:
            self.y = 0
            self.speed *= 0.5  # Rebond
        elif self.y > HEIGHT:
            self.y = HEIGHT
            self.speed *= 0.5  # Rebond
        
        # Mise à jour de l'image et du rectangle
        self.image = pygame.transform.rotate(self.original_image, self.angle)
        self.rect = self.image.get_rect(center=(self.x, self.y))

class Game:
    def __init__(self):
        self.state = MENU
        self.current_level = 1
        self.max_levels = 3
        self.track = Track(self.current_level)
        self.car = Car(*self.track.start_position, RED, YELLOW, self.track.start_angle)
        self.ai_cars = []
        self.create_ai_cars()
        self.game_over = False
        self.start_time = pygame.time.get_ticks()
        self.elapsed_time = 0
        self.best_times = [None] * self.max_levels
        self.font = pygame.font.SysFont(None, 36)
        self.small_font = pygame.font.SysFont(None, 24)
        self.car_crossed_finish_line = False  # Variable pour éviter les comptages multiples
    
    def create_ai_cars(self):
        self.ai_cars = []
        # Créer 3 adversaires avec des couleurs et difficultés différentes
        colors = [ORANGE, PURPLE, CYAN]
        start_x, start_y = self.track.start_position
        
        for i in range(3):
            # Positionner les voitures AI légèrement décalées
            x = start_x + (i+1) * 40
            y = start_y
            difficulty = min(i+1, 3)  # Difficulté croissante (1-3)
            self.ai_cars.append(AI_Car(x, y, self.track, colors[i], WHITE, difficulty, self.track.start_angle))
    
    def reset_level(self):
        self.track = Track(self.current_level)
        self.car = Car(*self.track.start_position, RED, YELLOW, self.track.start_angle)
        self.create_ai_cars()
        self.start_time = pygame.time.get_ticks()
        self.elapsed_time = 0
        self.state = PLAYING
    
    def next_level(self):
        if self.current_level < self.max_levels:
            self.current_level += 1
            self.reset_level()
        else:
            self.state = MENU  # Retour au menu si tous les niveaux sont terminés
    
    def update(self):
        if self.state == PLAYING:
            keys = pygame.key.get_pressed()
            self.car.update(keys)
            
            # Mettre à jour les voitures AI
            for ai_car in self.ai_cars:
                ai_car.update()
                
                # Vérifier les collisions entre voitures
                if self.car.rect.colliderect(ai_car.rect):
                    self.car.speed *= 0.8  # Ralentissement en cas de collision
                    ai_car.speed *= 0.8
            
            # Vérifier les collisions avec le circuit
            if self.track.check_collision(self.car.rect):
                self.car.speed *= 0.7  # Ralentissement sur l'herbe ou collision
            
            # Vérifier les collisions des AI avec le circuit
            for ai_car in self.ai_cars:
                if self.track.check_collision(ai_car.rect):
                    ai_car.speed *= 0.7
            
            # Vérifier si le joueur passe par les checkpoints
            for i, checkpoint in enumerate(self.track.checkpoints):
                # Si c'est le prochain checkpoint à atteindre
                if i == self.car.checkpoint_index:
                    # Calculer la distance au checkpoint
                    dx = checkpoint[0] - self.car.x
                    dy = checkpoint[1] - self.car.y
                    distance = math.sqrt(dx**2 + dy**2)
                    
                    # Si le joueur est assez proche du checkpoint
                    if distance < 50:  # Rayon du checkpoint
                        self.car.checkpoint_index += 1
                        # Si on a atteint le dernier checkpoint, on se prépare à compléter un tour
                        if self.car.checkpoint_index >= len(self.track.checkpoints):
                            self.car.checkpoint_index = 0
            
            # Vérifier si le joueur a terminé un tour
            # Logique améliorée: si le joueur passe la ligne de départ
            start_x, start_y = self.track.start_position
            near_finish_line = abs(self.car.x - start_x) < 30 and abs(self.car.y - start_y) < 30
            
            # Si le joueur est près de la ligne d'arrivée et n'a pas déjà été compté
            if near_finish_line and not self.car_crossed_finish_line:
                # Vérifier si le joueur a fait un tour complet
                if self.car.checkpoint_index >= len(self.track.checkpoints) - 1:
                    self.car.laps_completed += 1
                    self.car.checkpoint_index = 0
                    
                    # Si le joueur a terminé 3 tours, niveau terminé
                    if self.car.laps_completed >= 3:
                        # Enregistrer le meilleur temps
                        if self.best_times[self.current_level-1] is None or self.elapsed_time < self.best_times[self.current_level-1]:
                            self.best_times[self.current_level-1] = self.elapsed_time
                        self.state = LEVEL_COMPLETE
                
                # Marquer que le joueur a traversé la ligne
                self.car_crossed_finish_line = True
            # Si le joueur s'éloigne de la ligne d'arrivée, réinitialiser le marqueur
            elif not near_finish_line and self.car_crossed_finish_line:
                self.car_crossed_finish_line = False
            
            # Mettre à jour le temps écoulé
            self.elapsed_time = (pygame.time.get_ticks() - self.start_time) // 1000
    
    def draw(self):
        screen.fill(BLACK)
        
        if self.state == MENU:
            # Afficher le menu
            title = self.font.render("MicroMachines Python", True, WHITE)
            screen.blit(title, (WIDTH//2 - title.get_width()//2, 100))
            
            # Afficher les niveaux disponibles
            for i in range(self.max_levels):
                level_text = self.font.render(f"Niveau {i+1}", True, WHITE)
                level_rect = level_text.get_rect(center=(WIDTH//2, 200 + i*60))
                pygame.draw.rect(screen, GRAY, (level_rect.x - 10, level_rect.y - 10, level_rect.width + 20, level_rect.height + 20))
                screen.blit(level_text, level_rect)
                
                # Afficher le meilleur temps pour ce niveau
                if self.best_times[i] is not None:
                    time_text = self.small_font.render(f"Meilleur temps: {self.best_times[i]}s", True, YELLOW)
                    screen.blit(time_text, (level_rect.centerx - time_text.get_width()//2, level_rect.bottom + 10))
            
            # Instructions
            instructions = self.small_font.render("Appuyez sur 1, 2 ou 3 pour sélectionner un niveau", True, WHITE)
            screen.blit(instructions, (WIDTH//2 - instructions.get_width()//2, HEIGHT - 50))
            
        elif self.state == PLAYING:
            # Afficher le jeu
            self.track.draw(screen)
            
            # Dessiner les voitures AI
            for ai_car in self.ai_cars:
                ai_car.draw(screen)
            
            # Dessiner la voiture du joueur
            self.car.draw(screen)
            
            # Afficher le temps et le niveau
            time_text = self.font.render(f"Temps: {self.elapsed_time}s", True, WHITE)
            screen.blit(time_text, (10, 10))
            
            level_text = self.font.render(f"Niveau: {self.current_level}", True, WHITE)
            screen.blit(level_text, (WIDTH - level_text.get_width() - 10, 10))
            
            # Afficher le nombre de tours
            laps_text = self.font.render(f"Tours: {self.car.laps_completed}/3", True, WHITE)
            screen.blit(laps_text, (WIDTH//2 - laps_text.get_width()//2, 10))
            
            # Afficher les instructions
            instructions = self.small_font.render("ESPACE: Accélérer | ALT: Freiner | Flèches: Direction | ESC: Menu", True, WHITE)
            screen.blit(instructions, (WIDTH//2 - instructions.get_width()//2, HEIGHT - 30))
            
        elif self.state == LEVEL_COMPLETE:
            # Afficher l'écran de fin de niveau
            complete_text = self.font.render(f"Niveau {self.current_level} terminé!", True, WHITE)
            screen.blit(complete_text, (WIDTH//2 - complete_text.get_width()//2, HEIGHT//2 - 50))
            
            time_text = self.font.render(f"Temps: {self.elapsed_time}s", True, WHITE)
            screen.blit(time_text, (WIDTH//2 - time_text.get_width()//2, HEIGHT//2))
            
            if self.current_level < self.max_levels:
                next_text = self.font.render("Appuyez sur ESPACE pour le niveau suivant", True, WHITE)
                screen.blit(next_text, (WIDTH//2 - next_text.get_width()//2, HEIGHT//2 + 50))
            else:
                next_text = self.font.render("Félicitations! Vous avez terminé tous les niveaux!", True, YELLOW)
                screen.blit(next_text, (WIDTH//2 - next_text.get_width()//2, HEIGHT//2 + 50))
                menu_text = self.font.render("Appuyez sur ESPACE pour revenir au menu", True, WHITE)
                screen.blit(menu_text, (WIDTH//2 - menu_text.get_width()//2, HEIGHT//2 + 100))
        
        pygame.display.flip()

# Fonction principale
def main():
    game = Game()
    running = True
    
    # Variables pour stocker les rectangles des boutons de niveau
    level_buttons = [None] * game.max_levels
    
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    if game.state == PLAYING:
                        game.state = MENU
                    else:
                        running = False
                        
                # Sélection de niveau dans le menu avec le clavier
                if game.state == MENU:
                    if event.key == pygame.K_1:
                        game.current_level = 1
                        game.reset_level()
                    elif event.key == pygame.K_2:
                        game.current_level = 2
                        game.reset_level()
                    elif event.key == pygame.K_3:
                        game.current_level = 3
                        game.reset_level()
                
                # Passer au niveau suivant après avoir terminé un niveau
                if game.state == LEVEL_COMPLETE and event.key == pygame.K_SPACE:
                    if game.current_level < game.max_levels:
                        game.next_level()
                    else:
                        game.state = MENU
            
            # Détection des clics de souris sur les boutons de niveau
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:  # Clic gauche
                if game.state == MENU:
                    mouse_pos = pygame.mouse.get_pos()
                    
                    # Mettre à jour les rectangles des boutons de niveau
                    for i in range(game.max_levels):
                        level_text = game.font.render(f"Niveau {i+1}", True, WHITE)
                        level_rect = level_text.get_rect(center=(WIDTH//2, 200 + i*60))
                        # Agrandir le rectangle pour correspondre au bouton dessiné
                        button_rect = pygame.Rect(level_rect.x - 10, level_rect.y - 10, 
                                                level_rect.width + 20, level_rect.height + 20)
                        level_buttons[i] = button_rect
                    
                    # Vérifier si le clic est sur un des boutons de niveau
                    for i in range(game.max_levels):
                        if level_buttons[i].collidepoint(mouse_pos):
                            game.current_level = i + 1
                            game.reset_level()
                            break
        
        game.update()
        game.draw()
        clock.tick(FPS)
    
    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    main()
