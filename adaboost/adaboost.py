#################################
# Your name: Tal Trotskovsky
#################################

# Please import and use stuff only from the packages numpy, sklearn, matplotlib.
from matplotlib import pyplot as plt
import numpy as np
from process_data import parse_data

np.random.seed(7)


def optimal_index_and_mistake(cur_xi_yi, minus_plus_one):
    cur_mistake = 0
    optimal_index = -1
    for i in range(len(cur_xi_yi)):
        if(cur_xi_yi[i][1] == minus_plus_one):
            cur_mistake += cur_xi_yi[i][2]
    optimal_mistake = cur_mistake
    for i in range(len(cur_xi_yi)-1):
        if(cur_xi_yi[i][1] == minus_plus_one):
            cur_mistake -= cur_xi_yi[i][2]
        else:
            cur_mistake += cur_xi_yi[i][2]
        if(cur_mistake < optimal_mistake and cur_xi_yi[i][0] != cur_xi_yi[i + 1][0]):
            optimal_mistake = cur_mistake
            optimal_index = i
    if (cur_xi_yi[len(cur_xi_yi) - 1][1] == minus_plus_one):
        cur_mistake -= cur_xi_yi[len(cur_xi_yi) - 1][2]
    else:
        cur_mistake += cur_xi_yi[i][2]
    if(cur_mistake < optimal_mistake):
        optimal_mistake = cur_mistake
        optimal_index = i
    return optimal_index, optimal_mistake


def WL(X_train, y_train, D_t):
    optimal_mistake = None
    optimal_theta = None
    optimal_index = None
    optimal_minus_plus = None
    for i in range(X_train.shape[1]):
        #cur_xi_row = [X_train[j][i] for j in range(X_train.shape[0])]
        cur_xi_row = X_train[:,i]
        cur_xi_yi = list(zip(cur_xi_row, y_train, D_t))
        cur_xi_yi = sorted(cur_xi_yi, key = lambda x:x[0])
        cur_optimal_index_plus_one, mistake_for_index_plus_one = optimal_index_and_mistake(cur_xi_yi, 1)
        cur_optimal_index_minus_one, mistake_for_index_minus_one = optimal_index_and_mistake(cur_xi_yi, -1)
        if(mistake_for_index_plus_one < mistake_for_index_minus_one):
            cur_optimal_index = cur_optimal_index_plus_one
            mistake_for_index = mistake_for_index_plus_one
            cur_minus_plus = 1
        else:
            cur_optimal_index = cur_optimal_index_minus_one
            mistake_for_index = mistake_for_index_minus_one
            cur_minus_plus = -1
        if(optimal_mistake == None or mistake_for_index < optimal_mistake):
            optimal_index = i
            optimal_mistake = mistake_for_index
            optimal_minus_plus = cur_minus_plus
            if(cur_optimal_index == -1):
                optimal_theta = cur_xi_yi[0][0] - 0.1
            elif(cur_optimal_index == len(cur_xi_yi)-1):
                optimal_theta = cur_xi_yi[len(cur_xi_yi)-1][0] + 0.1
            else:
                optimal_theta = (cur_xi_yi[cur_optimal_index][0] + cur_xi_yi[cur_optimal_index + 1][0])/2

    classifier = [optimal_theta, optimal_index, optimal_mistake, optimal_minus_plus]
    return classifier

def classify(x_i, classifier):
    theta = classifier[0]
    index = classifier[1]
    plus_minus = classifier[3]
    if((plus_minus == 1 and x_i[index] <= theta) or (plus_minus == -1 and x_i[index] >= theta)):
        return 1
    return -1

def classify_h_t(x_i, h_t):
    plus_minus = h_t[0]
    index = h_t[1]
    theta = h_t[2]
    if ((plus_minus == 1 and x_i[index] <= theta) or (plus_minus == -1 and x_i[index] >= theta)):
        return 1
    return -1


def calc_next_D_t(D_t, X_train, y_train, w_t, h_t):
    D_t_new = [None]*len(D_t)
    Z=0
    for i in range(len(X_train)):
        sum_i = D_t[i] * np.exp(-w_t * y_train[i] * classify_h_t(X_train[i], h_t))
        Z += sum_i
        D_t_new[i] = sum_i
    D_t_new = np.array(D_t_new)
    D_t_new = D_t_new*(1/Z)
    return D_t_new

def run_adaboost(X_train, y_train, T):
    """
    Returns:

        hypotheses :
            A list of T tuples describing the hypotheses chosen by the algorithm.
            Each tuple has 3 elements (h_pred, h_index, h_theta), where h_pred is
            the returned value (+1 or -1) if the count at index h_index is <= h_theta.

        alpha_vals :
            A list of T float values, which are the alpha values obtained in every
            iteration of the algorithm.
    """
    D_t = [1/len(y_train) for i in range(len(y_train))]
    h_t_list = list()
    w_t_list = list()
    for t in range(T):
        WeakLearner = WL(X_train,y_train, D_t)
        h_t = (WeakLearner[3], WeakLearner[1], WeakLearner[0])
        h_t_list.append(h_t)
        epsilon_t = WeakLearner[2]
        w_t = 0.5*(np.log((1-epsilon_t)/epsilon_t))
        w_t_list.append(w_t)
        D_t = calc_next_D_t(D_t, X_train, y_train, w_t, h_t)
    return h_t_list, w_t_list


##############################################
# You can add more methods here, if needed.
def calc_g_t_for_x_i(x_i, alpha_vals, hypotheses, t):
    g_t = 0
    for i in range(t+1):
        g_t += alpha_vals[i]*classify_h_t(x_i, hypotheses[i])
    if(g_t >= 0):
        return 1
    return -1

def calc_error(X, y, alpha_vals, hypotheses, t):
    count_errors = 0
    for i in range(len(X)):
        current_label = calc_g_t_for_x_i(X[i],alpha_vals,hypotheses, t)
        if(current_label != y[i]):
            count_errors += 1
    error = count_errors/len(X)
    return error


def q_a(hypotheses, alpha_vals, X_train, y_train, X_test, y_test):
    T_list = [i for i in range(len(hypotheses))]
    train_error_list = list()
    test_error_list = list()
    for i in range(len(T_list)):
        cur_training_error = calc_error(X_train, y_train, alpha_vals, hypotheses, i)
        cur_test_error = calc_error(X_test, y_test, alpha_vals, hypotheses, i)
        train_error_list.append(cur_training_error)
        test_error_list.append(cur_test_error)
    plt.plot(T_list, train_error_list, color = "blue", label = "training error")
    plt.plot(T_list, test_error_list, color = "green", label = "test error")
    plt.xlabel("t (iterations)")
    plt.ylabel("error")
    #plt.legend()
    #plt.show()

def q_b(hypotheses):
    for i in range(len(hypotheses)):
        print(hypotheses[i])

def q_c(X_train, y_train, X_test, y_test, hypotheses, alpha_vals):
    T_list = [i for i in range(len(hypotheses))]
    l_train_error_list = list()
    l_test_error_list = list()
    for t in range(len(T_list)):
        cur_sum = 0
        for i in range(len(X_train)):
            cur_sub_sum = 0
            for j in range(t+1):
                cur_sub_sum += alpha_vals[j]*classify_h_t(X_train[i], hypotheses[j])
            cur_sum += np.exp(-y_train[i]*cur_sub_sum)
        l_train_error_list.append(cur_sum/len(X_train))
    for t in range(len(T_list)):
        cur_sum = 0
        for i in range(len(X_test)):
            cur_sub_sum = 0
            for j in range(t+1):
                cur_sub_sum += alpha_vals[j]*classify_h_t(X_test[i], hypotheses[j])
            cur_sum += np.exp(-y_test[i]*cur_sub_sum)
        l_test_error_list.append(cur_sum/len(X_test))
    plt.plot(T_list, l_train_error_list, color="blue", label="training error")
    plt.plot(T_list, l_test_error_list, color="green", label="test error")
    plt.xlabel("t (iterations)")
    plt.ylabel("exp error")
    #plt.legend()
    #plt.show()

##############################################


def main():
    data = parse_data()
    if not data:
        return
    (X_train, y_train, X_test, y_test, vocab) = data
    hypotheses, alpha_vals = run_adaboost(X_train, y_train, 80)
    q_a(hypotheses, alpha_vals, X_train, y_train, X_test, y_test)

    ##############################################
    # You can add more methods here, if needed.
    hypotheses2, alpha_vals = run_adaboost(X_train, y_train, 10)
    #q_b(hypotheses2)
    #q_c(X_train, y_train, X_test, y_test, hypotheses, alpha_vals)
    ##############################################

if __name__ == '__main__':
    main()



