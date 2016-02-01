#include "millstate.hh"

MillState::MillState()
{
	for (int i = 0; i < 24; ++i) cs[i] = -1;
	notplaced[0] = notplaced[1] = 9;
	onboard[0]   = onboard[1]   = 0;
	killed[0]    = killed[1]    = 0;
}

void MillState::add(int player, int pos)
{
	if (notplaced[player] > 0 && cs[pos] == -1) {
		notplaced[player]--;
		onboard[player]++;
		cs[pos] = player;
	}
}

void MillState::move(int from, int to)
{
	if (cs[from] != -1 && cs[to] == -1) {
		cs[to] = cs[from];
		cs[from] = -1;
	}
}

void MillState::remove(int pos)
{
	if (cs[pos] != -1) {
		int player = cs[pos];
		onboard[player]--;
		killed[player]++;
		cs[pos] = -1;
	}
}

bool MillState::ismill(int pos) const
{
	for (int i : {0, 1}) {
		const int *m = millsfrom[pos][i];
		if (cs[m[0]] == cs[m[1]] && cs[m[0]] == cs[m[2]]) return true;
	}
	return false;
}

QVector<int> MillState::eatable(int player) const
{
	QVector<int> eas;
	for (int i = 0; i < 24; ++i) {
		if (cs[i] == player && !ismill(i)) eas.append(i);
	}
	return eas;
}
